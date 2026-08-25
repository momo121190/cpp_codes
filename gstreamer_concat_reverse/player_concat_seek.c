/*
 * player_concat_seek.c
 *
 * ============================================================
 *  PRINCIPE
 * ============================================================
 *
 *  Le code source de concat (gstconcat.c) montre que :
 *
 *   - gst_concat_src_event() ne pousse un SEEK que vers
 *     current_sinkpad : un seek "global" sur le pipeline
 *     n'atteint jamais les autres branches.
 *   - gst_concat_src_query() ne traite pas DURATION : concat
 *     rapporte toujours la durée du seul segment courant.
 *   - gst_concat_switch_pad() ne change de branche que sur EOS,
 *     et toujours dans l'ordre où les pads ont été demandés.
 *
 *  Conclusion : on n'essaie PAS de faire traduire une position
 *  globale par concat. À la place :
 *
 *   1. On construit le pipeline avec uniquement les séquences
 *      concernées, DANS L'ORDRE DE LECTURE VOULU (inversé pour
 *      un reverse).
 *   2. On envoie un seek à CHAQUE BRANCHE individuellement (sur
 *      son qtdemux, par son nom), pendant que le pipeline est en
 *      PAUSED.
 *   3. La première branche est bornée à l'instant demandé ; les
 *      suivantes sont intégrales.
 *   4. concat enchaîne ensuite les branches sur EOS — c'est
 *      exactement ce pour quoi il est conçu.
 *
 *  Exemple : reverse à 5s de SEQ 2
 *      branche 0 : seq2.mp4  rate=-1  start=0  stop=5s
 *      branche 1 : seq1.mp4  rate=-1  start=0  stop=10s
 *    -> SEQ 2 joue de 5s vers 0, EOS, concat bascule,
 *       SEQ 1 joue de 10s vers 0, EOS final.
 *
 * ============================================================
 *
 *  Compilation : make
 *  Lancement   : ./player [port]      (défaut 5000)
 */

#include <gst/gst.h>
#include <glib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DEFAULT_PORT 5000
#define MAX_BRANCH   16
#define POLL_MS      200

/* ---------- couleurs ---------- */
#define C_RESET "\033[0m"
#define C_DIM   "\033[2m"
#define C_RED   "\033[31m"
#define C_GRN   "\033[32m"
#define C_YEL   "\033[33m"
#define C_BLU   "\033[34m"
#define C_MAG   "\033[35m"
#define C_CYN   "\033[36m"
#define C_BOLD  "\033[1m"

typedef struct {
    const char *filename;
    const char *label;
    gdouble start;      /* position sur la timeline globale */
    gdouble duration;
} Segment;

/* Table des séquences. Dans ton service replay, à construire
 * dynamiquement à partir des métadonnées réelles. */
static Segment segments[] = {
    { "seq1.mp4", "SEQ 1",  0.0, 10.0 },
    { "seq2.mp4", "SEQ 2", 10.0,  8.0 },
    { "seq3.mp4", "SEQ 3", 18.0, 15.0 },
};
#define N_SEG ((int)(sizeof(segments)/sizeof(segments[0])))
#define TOTAL (segments[N_SEG-1].start + segments[N_SEG-1].duration)

/* Description d'une branche du pipeline courant */
typedef struct {
    int      seg_index;      /* index dans segments[] */
    gdouble  play_from;      /* borne haute (reverse) ou basse (forward), en local */
    gdouble  play_span;      /* durée réellement jouée par cette branche */
    gdouble  cumul_before;   /* somme des spans des branches précédentes */
    char     demux_name[16]; /* nom de l'élément qtdemux, ex "d0" */
} Branch;

static GstElement *pipeline = NULL;
static Branch      branches[MAX_BRANCH];
static int         n_branches = 0;
static gdouble     current_rate = 1.0;
static int         active_branch = 0;
static gchar      *base_dir = NULL;

/* Évite de reseeker plusieurs fois la même branche (boucle infinie) */
static gboolean    reseeked[MAX_BRANCH];

/* ================== affichage ================== */

static int segment_at(gdouble t) {
    for (int i = 0; i < N_SEG; i++)
        if (t >= segments[i].start && t < segments[i].start + segments[i].duration)
            return i;
    return -1;
}

static void timeline_bar(gdouble pos, char *out, size_t sz) {
    const int W = 46;
    char bar[128];
    for (int i = 0; i < W; i++) bar[i] = '.';
    bar[W] = 0;
    for (int s = 1; s < N_SEG; s++) {
        int c = (int)((segments[s].start / TOTAL) * W);
        if (c > 0 && c < W) bar[c] = '|';
    }
    if (pos >= 0) {
        int c = (int)((pos / TOTAL) * W);
        if (c < 0) c = 0;
        if (c >= W) c = W - 1;
        bar[c] = '#';
    }
    snprintf(out, sz, "[%s]", bar);
}

/* Traduit la position rapportée par le pipeline en position globale.
 *
 * IMPORTANT : gst_concat_src_query() ne traite pas la requête POSITION,
 * elle est transmise par gst_pad_query_default() vers le seul fichier
 * courant. concat rapporte donc la position DANS LE FICHIER EN COURS,
 * jamais une position cumulée sur la timeline.
 *
 * La conversion est donc :
 *     global = début du segment de la branche active + position locale
 * et c'est le suivi de active-pad qui nous dit quelle branche est active. */
static gdouble concat_pos_to_global(gdouble p) {
    if (n_branches == 0 || p < 0) return -1.0;
    if (active_branch < 0 || active_branch >= n_branches) return -1.0;

    Segment *s = &segments[branches[active_branch].seg_index];
    gdouble local = p;
    if (local < 0) local = 0;
    if (local > s->duration) local = s->duration;
    return s->start + local;
}

static gdouble get_global_position(void) {
    gint64 ns = 0;
    if (!pipeline) return -1.0;
    if (!gst_element_query_position(pipeline, GST_FORMAT_TIME, &ns)) return -1.0;
    return concat_pos_to_global((gdouble)ns / GST_SECOND);
}

static void show_state(const char *tag) {
    gdouble g = get_global_position();
    int seg = segment_at(g);
    char bar[128];
    timeline_bar(g, bar, sizeof(bar));
    g_print("%s%-10s%s %s %s%6.2fs%s/%.0fs  %s  %s%s%s\n",
            C_BOLD, tag, C_RESET, bar,
            C_CYN, g, C_RESET, TOTAL,
            current_rate < 0 ? C_YEL "<<REV" C_RESET : C_GRN ">>FWD" C_RESET,
            C_DIM, seg >= 0 ? segments[seg].label : "?", C_RESET);
}

/* Affiche le plan de lecture avant de lancer : très utile pour
 * comprendre ce que le pipeline va faire. */
static void print_plan(void) {
    g_print("\n%s┌─ PLAN DE LECTURE ─────────────────────────────────────%s\n",
            C_MAG, C_RESET);
    g_print("%s│%s sens : %s   branches : %d\n", C_MAG, C_RESET,
            current_rate < 0 ? C_YEL "REVERSE" C_RESET : C_GRN "FORWARD" C_RESET,
            n_branches);
    for (int i = 0; i < n_branches; i++) {
        Segment *s = &segments[branches[i].seg_index];
        if (current_rate < 0) {
            g_print("%s│%s  [%d] %s%-6s%s %-10s  joue %s%.2fs -> 0.00s%s "
                    "(span %.2fs)  %s\n",
                    C_MAG, C_RESET, i, C_CYN, s->label, C_RESET, s->filename,
                    C_YEL, branches[i].play_from, C_RESET,
                    branches[i].play_span, branches[i].demux_name);
        } else {
            g_print("%s│%s  [%d] %s%-6s%s %-10s  joue %s%.2fs -> %.2fs%s "
                    "(span %.2fs)  %s\n",
                    C_MAG, C_RESET, i, C_CYN, s->label, C_RESET, s->filename,
                    C_GRN, branches[i].play_from, s->duration, C_RESET,
                    branches[i].play_span, branches[i].demux_name);
        }
    }
    g_print("%s└───────────────────────────────────────────────────────%s\n\n",
            C_MAG, C_RESET);
}

/* ================== réseau ================== */

static void reply(int fd, const char *fmt, ...) {
    if (fd < 0) return;
    char buf[600];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    strncat(buf, "\n", sizeof(buf) - strlen(buf) - 1);
    if (write(fd, buf, strlen(buf)) < 0) { /* ignore */ }
}

/* ================== notification de bascule de branche ================== */

/* Réapplique le seek sur la branche qui vient de devenir courante.
 *
 * Nécessaire parce que concat bloque les branches non courantes
 * (gst_concat_pad_wait) : le seek envoyé en PAUSED ne survit pas
 * toujours au déblocage, et la branche repart alors en avant ou
 * émet un EOS immédiat. On le rejoue donc au bon moment.
 * Exécuté via g_idle_add : on ne seek pas depuis le callback GObject. */
static gboolean reseek_branch(gpointer data) {
    int idx = GPOINTER_TO_INT(data);
    if (!pipeline || idx < 0 || idx >= n_branches) return G_SOURCE_REMOVE;
    if (reseeked[idx]) return G_SOURCE_REMOVE;
    reseeked[idx] = TRUE;

    GstElement *demux =
        gst_bin_get_by_name(GST_BIN(pipeline), branches[idx].demux_name);
    if (!demux) return G_SOURCE_REMOVE;

    Segment *s = &segments[branches[idx].seg_index];
    gboolean ok;

    if (current_rate < 0) {
        ok = gst_element_seek(demux, current_rate, GST_FORMAT_TIME,
                GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
                GST_SEEK_TYPE_SET, 0,
                GST_SEEK_TYPE_SET,
                (gint64)(branches[idx].play_from * GST_SECOND));
    } else {
        ok = gst_element_seek(demux, current_rate, GST_FORMAT_TIME,
                GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
                GST_SEEK_TYPE_SET,
                (gint64)(branches[idx].play_from * GST_SECOND),
                GST_SEEK_TYPE_SET, GST_CLOCK_TIME_NONE);
    }

    g_print("%s[re-seek]%s branche %d (%s%s%s) rate=%.1f borne=%.2fs  %s\n",
            C_YEL, C_RESET, idx, C_CYN, s->label, C_RESET,
            current_rate, branches[idx].play_from,
            ok ? C_GRN "OK" C_RESET : C_RED "REFUSE" C_RESET);

    gst_object_unref(demux);
    return G_SOURCE_REMOVE;
}

static void on_active_pad(GstElement *concat, GParamSpec *pspec, gpointer data) {
    (void)pspec; (void)data;
    GstPad *pad = NULL;
    g_object_get(concat, "active-pad", &pad, NULL);
    if (!pad) return;

    const gchar *name = GST_PAD_NAME(pad);
    /* sink_0 -> branche 0, sink_1 -> branche 1, ... */
    int idx = -1;
    if (sscanf(name, "sink_%d", &idx) == 1 && idx >= 0 && idx < n_branches) {
        active_branch = idx;
        Segment *s = &segments[branches[idx].seg_index];
        g_print("%s[bascule]%s concat -> branche %d  (%s%s%s, %s)\n",
                C_BLU, C_RESET, idx, C_CYN, s->label, C_RESET, s->filename);

        /* la branche 0 a déjà été seekée juste avant PLAYING ;
         * les suivantes sont rejouées au moment de leur activation */
        if (idx > 0)
            g_idle_add(reseek_branch, GINT_TO_POINTER(idx));
    }
    gst_object_unref(pad);
}

/* ================== construction du pipeline ================== */

static void teardown(void) {
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = NULL;
    }
    n_branches = 0;
    active_branch = 0;
    for (int i = 0; i < MAX_BRANCH; i++) reseeked[i] = FALSE;
}

static gboolean bus_watch_cb(GstBus *bus, GstMessage *msg, gpointer data);

/*
 * Construit un pipeline concat contenant uniquement les séquences
 * à lire, dans l'ordre de lecture, et applique un seek à chaque
 * branche séparément.
 *
 *   seg_index : séquence où se trouve le point de départ
 *   local_t   : instant DANS cette séquence (secondes)
 *   rate      : -1.0 pour reverse, +1.0 pour forward
 */
static gboolean build_and_start(int seg_index, gdouble local_t, gdouble rate) {
    teardown();
    current_rate = rate;

    /* --- 1. établir la liste ordonnée des branches --- */
    n_branches = 0;
    gdouble cumul = 0.0;

    if (rate < 0) {
        /* reverse : seg_index, puis les précédentes, jusqu'à 0 */
        for (int i = seg_index; i >= 0 && n_branches < MAX_BRANCH; i--) {
            Branch *b = &branches[n_branches];
            b->seg_index = i;
            b->play_from = (i == seg_index) ? local_t : segments[i].duration;
            b->play_span = b->play_from;         /* on descend jusqu'à 0 */
            b->cumul_before = cumul;
            snprintf(b->demux_name, sizeof(b->demux_name), "d%d", n_branches);
            cumul += b->play_span;
            n_branches++;
        }
    } else {
        /* forward : seg_index, puis les suivantes, jusqu'à la fin */
        for (int i = seg_index; i < N_SEG && n_branches < MAX_BRANCH; i++) {
            Branch *b = &branches[n_branches];
            b->seg_index = i;
            b->play_from = (i == seg_index) ? local_t : 0.0;
            b->play_span = segments[i].duration - b->play_from;
            b->cumul_before = cumul;
            snprintf(b->demux_name, sizeof(b->demux_name), "d%d", n_branches);
            cumul += b->play_span;
            n_branches++;
        }
    }

    if (n_branches == 0) {
        g_print("%s[!] aucune branche à construire%s\n", C_RED, C_RESET);
        return FALSE;
    }

    print_plan();

    /* --- 2. construire la description du pipeline --- */
    GString *desc = g_string_new(
        "concat name=c ! h264parse ! avdec_h264 ! videoconvert ! ximagesink ");

    for (int i = 0; i < n_branches; i++) {
        Segment *s = &segments[branches[i].seg_index];
        g_string_append_printf(desc,
            "filesrc location=%s ! qtdemux name=%s ! queue ! c. ",
            s->filename, branches[i].demux_name);
    }

    g_print("%s[pipeline]%s %s%s%s\n\n", C_BLU, C_RESET, C_DIM, desc->str, C_RESET);

    GError *err = NULL;
    pipeline = gst_parse_launch(desc->str, &err);
    g_string_free(desc, TRUE);

    if (!pipeline) {
        g_print("%s[!] construction échouée : %s%s\n",
                C_RED, err ? err->message : "?", C_RESET);
        if (err) g_error_free(err);
        return FALSE;
    }

    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, bus_watch_cb, NULL);
    gst_object_unref(bus);

    /* suivi des bascules de branche */
    GstElement *concat = gst_bin_get_by_name(GST_BIN(pipeline), "c");
    if (concat) {
        g_signal_connect(concat, "notify::active-pad",
                         G_CALLBACK(on_active_pad), NULL);
        gst_object_unref(concat);
    }

    /* --- 3. PAUSED : les demuxers analysent leurs fichiers --- */
    gst_element_set_state(pipeline, GST_STATE_PAUSED);
    GstStateChangeReturn sret =
        gst_element_get_state(pipeline, NULL, NULL, 5 * GST_SECOND);
    if (sret == GST_STATE_CHANGE_FAILURE) {
        g_print("%s[!] passage en PAUSED échoué%s\n", C_RED, C_RESET);
        return FALSE;
    }

    /* --- 4. seek INDIVIDUEL sur chaque branche --- *
     * C'est le coeur de la solution : concat ne propage un seek
     * qu'au segment courant, donc on s'adresse à chaque qtdemux. */
    g_print("%s┌─ SEEK PAR BRANCHE ────────────────────────────────────%s\n",
            C_MAG, C_RESET);

    for (int i = 0; i < n_branches; i++) {
        GstElement *demux =
            gst_bin_get_by_name(GST_BIN(pipeline), branches[i].demux_name);
        if (!demux) {
            g_print("%s│ [!] élément %s introuvable%s\n",
                    C_RED, branches[i].demux_name, C_RESET);
            continue;
        }

        Segment *s = &segments[branches[i].seg_index];
        gboolean ok;

        if (rate < 0) {
            /* reverse : lecture de stop vers start */
            gint64 stop_ns = (gint64)(branches[i].play_from * GST_SECOND);
            ok = gst_element_seek(demux, rate, GST_FORMAT_TIME,
                    GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
                    GST_SEEK_TYPE_SET, 0,
                    GST_SEEK_TYPE_SET, stop_ns);
            g_print("%s│%s [%d] %-4s %s%-6s%s  rate=%.1f  start=0.00s  stop=%.2fs   %s\n",
                    C_MAG, C_RESET, i, branches[i].demux_name,
                    C_CYN, s->label, C_RESET, rate, branches[i].play_from,
                    ok ? C_GRN "OK" C_RESET : C_RED "REFUSE" C_RESET);
        } else {
            /* forward : lecture de start vers la fin */
            gint64 start_ns = (gint64)(branches[i].play_from * GST_SECOND);
            ok = gst_element_seek(demux, rate, GST_FORMAT_TIME,
                    GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
                    GST_SEEK_TYPE_SET, start_ns,
                    GST_SEEK_TYPE_SET, GST_CLOCK_TIME_NONE);
            g_print("%s│%s [%d] %-4s %s%-6s%s  rate=%.1f  start=%.2fs  stop=fin   %s\n",
                    C_MAG, C_RESET, i, branches[i].demux_name,
                    C_CYN, s->label, C_RESET, rate, branches[i].play_from,
                    ok ? C_GRN "OK" C_RESET : C_RED "REFUSE" C_RESET);
        }
        gst_object_unref(demux);
    }
    g_print("%s└───────────────────────────────────────────────────────%s\n\n",
            C_MAG, C_RESET);

    /* --- 5. lancer --- */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    return TRUE;
}

static gboolean bus_watch_cb(GstBus *bus, GstMessage *msg, gpointer data) {
    (void)bus; (void)data;
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR: {
        GError *e; gchar *dbg;
        gst_message_parse_error(msg, &e, &dbg);
        g_print("%s[erreur]%s %s\n", C_RED, C_RESET, e->message);
        g_error_free(e); g_free(dbg);
        break;
    }
    case GST_MESSAGE_EOS:
        g_print("%s[EOS]%s toutes les branches ont été lues\n", C_BLU, C_RESET);
        show_state("FIN");
        break;
    default:
        break;
    }
    return TRUE;
}

/* ================== commandes ================== */

typedef struct { char line[256]; int fd; } Command;

static gboolean process_command(gpointer data) {
    Command *c = (Command *)data;
    char *line = c->line;
    int fd = c->fd;
    line[strcspn(line, "\r\n")] = 0;

    if (strncmp(line, "REVERSE", 7) == 0) {
        gdouble t;
        if (sscanf(line, "REVERSE %lf", &t) != 1) {
            t = get_global_position();
            if (t < 0) t = TOTAL;
        }
        int idx = segment_at(t);
        if (idx < 0) { reply(fd, "ERR position %.2f hors timeline", t); goto done; }
        gdouble local = t - segments[idx].start;
        g_print("\n%s══ REVERSE demandé à %.2fs (= %s + %.2fs) ══%s\n",
                C_BOLD, t, segments[idx].label, local, C_RESET);
        gboolean ok = build_and_start(idx, local, -1.0);
        reply(fd, "%s REVERSE depuis %.2f (%s local %.2f)",
              ok ? "OK" : "ERR", t, segments[idx].label, local);

    } else if (strncmp(line, "FORWARD", 7) == 0) {
        gdouble t;
        if (sscanf(line, "FORWARD %lf", &t) != 1) {
            t = get_global_position();
            if (t < 0) t = 0;
        }
        int idx = segment_at(t);
        if (idx < 0) { reply(fd, "ERR position %.2f hors timeline", t); goto done; }
        gdouble local = t - segments[idx].start;
        g_print("\n%s══ FORWARD demandé à %.2fs (= %s + %.2fs) ══%s\n",
                C_BOLD, t, segments[idx].label, local, C_RESET);
        gboolean ok = build_and_start(idx, local, 1.0);
        reply(fd, "%s FORWARD depuis %.2f (%s local %.2f)",
              ok ? "OK" : "ERR", t, segments[idx].label, local);

    } else if (strncmp(line, "PLAY", 4) == 0) {
        g_print("\n%s══ PLAY depuis le début ══%s\n", C_BOLD, C_RESET);
        gboolean ok = build_and_start(0, 0.0, 1.0);
        reply(fd, "%s PLAY", ok ? "OK" : "ERR");

    } else if (strncmp(line, "STATUS", 6) == 0) {
        gdouble g = get_global_position();
        int seg = segment_at(g);
        show_state("STATUS");
        reply(fd, "POS %.2f | RATE %.1f | SEGMENT %s | BRANCHE %d/%d",
              g, current_rate, seg >= 0 ? segments[seg].label : "?",
              active_branch, n_branches);

    } else if (strncmp(line, "PLAN", 4) == 0) {
        print_plan();
        reply(fd, "OK plan affiché côté serveur (%d branches)", n_branches);

    } else if (strncmp(line, "QUIT", 4) == 0) {
        reply(fd, "BYE");
    } else if (strlen(line) > 0) {
        reply(fd, "ERR commande inconnue: %s", line);
    }

done:
    g_free(c);
    return G_SOURCE_REMOVE;
}

static void *socket_thread(void *arg) {
    int port = *(int *)arg;
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in a = {0};
    a.sin_family = AF_INET; a.sin_addr.s_addr = INADDR_ANY; a.sin_port = htons(port);
    if (bind(sfd, (struct sockaddr *)&a, sizeof(a)) < 0) {
        perror("bind");
        g_print("Astuce : fuser -k %d/tcp\n", port);
        exit(1);
    }
    listen(sfd, 4);
    g_print("%s[réseau]%s port %d\n\n", C_BLU, C_RESET, port);

    while (1) {
        int cfd = accept(sfd, NULL, NULL);
        if (cfd < 0) continue;
        char buf[256]; ssize_t n;
        while ((n = read(cfd, buf, sizeof(buf) - 1)) > 0) {
            buf[n] = 0;
            char *sp; char *tok = strtok_r(buf, "\n", &sp);
            while (tok) {
                Command *c = g_new0(Command, 1);
                strncpy(c->line, tok, sizeof(c->line) - 1);
                c->fd = cfd;
                g_idle_add(process_command, c);
                tok = strtok_r(NULL, "\n", &sp);
            }
        }
        close(cfd);
    }
    return NULL;
}

static gboolean tick(gpointer d) {
    (void)d;
    return G_SOURCE_CONTINUE;   /* placeholder : le poll sert au client */
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);
    int port = DEFAULT_PORT;
    if (argc > 1) port = atoi(argv[1]);
    base_dir = g_get_current_dir();

    g_print("\n%s═══════════════════════════════════════════════════════%s\n",
            C_BOLD, C_RESET);
    g_print("%s  concat + seek par branche  —  reverse/forward à la demande%s\n",
            C_BOLD, C_RESET);
    g_print("%s═══════════════════════════════════════════════════════%s\n\n",
            C_BOLD, C_RESET);

    g_print("Timeline globale (%.0fs) :\n", TOTAL);
    for (int i = 0; i < N_SEG; i++)
        g_print("   %s%-6s%s %5.1fs -> %5.1fs   %s\n", C_CYN, segments[i].label,
                C_RESET, segments[i].start,
                segments[i].start + segments[i].duration, segments[i].filename);
    g_print("\nCommandes : PLAY / REVERSE [t] / FORWARD [t] / STATUS / PLAN\n");

    pthread_t tid;
    pthread_create(&tid, NULL, socket_thread, &port);
    g_timeout_add(POLL_MS, tick, NULL);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    teardown();
    g_free(base_dir);
    return 0;
}
