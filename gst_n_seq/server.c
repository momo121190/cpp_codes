/*
 * server.c — reverse / forward à un instant donné, avec concat
 *            nombre de séquences quelconque, découvert au démarrage
 *
 * Principe (voir README) :
 *   1. on ne met dans le pipeline que les séquences à lire, dans l'ordre
 *      de lecture (inversé pour un reverse)
 *   2. chaque branche reçoit son propre seek, envoyé à son qtdemux nommé
 *   3. le seek est rejoué quand concat bascule sur la branche
 *
 * Compilation : make
 * Lancement   : ./server [port]      (défaut 5000)
 */

#include <gst/gst.h>
#include <gst/pbutils/pbutils.h>
#include <glib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DEFAULT_PORT 5000
#define MAX_SEG 64

/* couleurs */
#define R "\033[0m"
#define B "\033[1m"
#define D "\033[2m"
#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define CYN "\033[36m"

typedef struct {
    gchar  *file;
    gdouble start;      /* offset sur la timeline globale */
    gdouble duration;
} Segment;

static Segment  seg[MAX_SEG];
static int      n_seg = 0;
static gdouble  total = 0;

/* plan de lecture courant */
static int      plan[MAX_SEG];    /* plan[b] = index dans seg[] */
static gdouble  bound[MAX_SEG];   /* borne de lecture de la branche b */
static gboolean done[MAX_SEG];    /* seek déjà rejoué pour la branche b */
static int      n_plan = 0;
static int      active = 0;

static GstElement *pipe_ = NULL;
static gdouble     rate = 1.0;

/* ================== découverte des séquences ================== */

static void discover(void) {
    GDir *dir = g_dir_open(".", 0, NULL);
    if (!dir) return;

    GList *files = NULL;
    const gchar *name;
    while ((name = g_dir_read_name(dir)))
        if (g_str_has_suffix(name, ".mp4"))
            files = g_list_insert_sorted(files, g_strdup(name),
                                         (GCompareFunc)g_strcmp0);
    g_dir_close(dir);

    GstDiscoverer *d = gst_discoverer_new(10 * GST_SECOND, NULL);
    gchar *cwd = g_get_current_dir();

    for (GList *l = files; l && n_seg < MAX_SEG; l = l->next) {
        gchar *uri = g_strdup_printf("file://%s/%s", cwd, (gchar *)l->data);
        GstDiscovererInfo *info = gst_discoverer_discover_uri(d, uri, NULL);
        if (info && gst_discoverer_info_get_result(info) == GST_DISCOVERER_OK) {
            seg[n_seg].file     = g_strdup((gchar *)l->data);
            seg[n_seg].duration = (gdouble)gst_discoverer_info_get_duration(info)
                                  / GST_SECOND;
            seg[n_seg].start    = total;
            total += seg[n_seg].duration;
            n_seg++;
        }
        if (info) gst_discoverer_info_unref(info);
        g_free(uri);
        g_free(l->data);
    }
    g_list_free(files);
    g_free(cwd);
    gst_object_unref(d);
}

static int seg_at(gdouble t) {
    for (int i = 0; i < n_seg; i++)
        if (t >= seg[i].start && t < seg[i].start + seg[i].duration) return i;
    return -1;
}

/* ================== réseau ================== */

static void reply(int fd, const char *fmt, ...) {
    if (fd < 0) return;
    char buf[2048];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    strncat(buf, "\n", sizeof(buf) - strlen(buf) - 1);
    if (write(fd, buf, strlen(buf)) < 0) { }
}

/* ================== seek par branche ================== */

static void seek_branch(int b) {
    gchar name[8];
    g_snprintf(name, sizeof(name), "d%d", b);
    GstElement *demux = gst_bin_get_by_name(GST_BIN(pipe_), name);
    if (!demux) {
        g_print("  %s[!] %s introuvable%s\n", RED, name, R);
        return;
    }

    gint64 ns = (gint64)(bound[b] * GST_SECOND);
    gboolean ok;

    if (rate < 0)   /* reverse : de bound vers 0 */
        ok = gst_element_seek(demux, rate, GST_FORMAT_TIME,
                GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
                GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_SET, ns);
    else            /* forward : de bound vers la fin */
        ok = gst_element_seek(demux, rate, GST_FORMAT_TIME,
                GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
                GST_SEEK_TYPE_SET, ns, GST_SEEK_TYPE_SET, GST_CLOCK_TIME_NONE);

    g_print("  %-3s %-12s rate=%+.0f borne=%6.2fs  %s\n",
            name, seg[plan[b]].file, rate, bound[b],
            ok ? GRN "ok" R : RED "REFUSE" R);
    gst_object_unref(demux);
}

static gboolean reseek_idle(gpointer p) {
    int b = GPOINTER_TO_INT(p);
    if (pipe_ && b < n_plan && !done[b]) {
        done[b] = TRUE;
        g_print("  %s[re-seek]%s ", YEL, R);
        seek_branch(b);
    }
    return G_SOURCE_REMOVE;
}

static void on_pad(GstElement *c, GParamSpec *ps, gpointer u) {
    (void)ps; (void)u;
    GstPad *pad = NULL;
    g_object_get(c, "active-pad", &pad, NULL);
    if (!pad) return;
    int b;
    if (sscanf(GST_PAD_NAME(pad), "sink_%d", &b) == 1 && b < n_plan) {
        active = b;
        g_print("%s[bascule]%s branche %d/%d -> %s%s%s\n",
                BLU, R, b, n_plan - 1, CYN, seg[plan[b]].file, R);
        if (b > 0) g_idle_add(reseek_idle, GINT_TO_POINTER(b));
    }
    gst_object_unref(pad);
}

/* ================== construction et lancement ================== */

static gboolean on_bus(GstBus *bus, GstMessage *m, gpointer u) {
    (void)bus; (void)u;
    if (GST_MESSAGE_TYPE(m) == GST_MESSAGE_ERROR) {
        GError *e; gst_message_parse_error(m, &e, NULL);
        g_print("%s[erreur]%s %s\n", RED, R, e->message);
        g_error_free(e);
    } else if (GST_MESSAGE_TYPE(m) == GST_MESSAGE_EOS) {
        g_print("%s[fin]%s toutes les branches lues\n", BLU, R);
    }
    return TRUE;
}

static gboolean play_from(gdouble t, gdouble r) {
    int idx = seg_at(t);
    if (idx < 0) {
        g_print("%s[!] position %.2fs hors timeline (0-%.2f)%s\n",
                RED, t, total, R);
        return FALSE;
    }

    if (pipe_) {
        gst_element_set_state(pipe_, GST_STATE_NULL);
        gst_object_unref(pipe_);
        pipe_ = NULL;
    }
    rate = r;
    n_plan = 0;
    active = 0;
    memset(done, 0, sizeof(done));

    gdouble local = t - seg[idx].start;

    /* branches, dans l'ordre de lecture */
    if (r < 0)
        for (int i = idx; i >= 0; i--) {
            plan[n_plan]  = i;
            bound[n_plan] = (i == idx) ? local : seg[i].duration;
            n_plan++;
        }
    else
        for (int i = idx; i < n_seg; i++) {
            plan[n_plan]  = i;
            bound[n_plan] = (i == idx) ? local : 0.0;
            n_plan++;
        }

    g_print("\n%s%s à %.2fs = %s + %.2fs — %d branche(s)%s\n",
            B, r < 0 ? "REVERSE" : "FORWARD", t, seg[idx].file, local, n_plan, R);

    /* description du pipeline. h264parse par branche : les séquences
     * peuvent avoir des SPS/PPS différents, on normalise avant concat. */
    GString *s = g_string_new(
        "concat name=c ! avdec_h264 ! videoconvert ! ximagesink ");
    for (int i = 0; i < n_plan; i++)
        g_string_append_printf(s,
            "filesrc location=%s ! qtdemux name=d%d ! h264parse ! queue ! c. ",
            seg[plan[i]].file, i);

    GError *err = NULL;
    pipe_ = gst_parse_launch(s->str, &err);
    g_string_free(s, TRUE);
    if (!pipe_) {
        g_print("%s[!] construction échouée : %s%s\n",
                RED, err ? err->message : "?", R);
        if (err) g_error_free(err);
        return FALSE;
    }

    GstBus *bus = gst_element_get_bus(pipe_);
    gst_bus_add_watch(bus, on_bus, NULL);
    gst_object_unref(bus);

    GstElement *c = gst_bin_get_by_name(GST_BIN(pipe_), "c");
    if (c) {
        g_signal_connect(c, "notify::active-pad", G_CALLBACK(on_pad), NULL);
        gst_object_unref(c);
    }

    /* timeout large : avec beaucoup de branches, tous les qtdemux
     * doivent analyser leur fichier avant que PAUSED soit atteint */
    gst_element_set_state(pipe_, GST_STATE_PAUSED);
    if (gst_element_get_state(pipe_, NULL, NULL, 20 * GST_SECOND)
        == GST_STATE_CHANGE_FAILURE) {
        g_print("%s[!] passage en PAUSED échoué%s\n", RED, R);
        return FALSE;
    }

    g_print("%sseeks :%s\n", D, R);
    for (int i = 0; i < n_plan; i++) seek_branch(i);
    done[0] = TRUE;

    gst_element_set_state(pipe_, GST_STATE_PLAYING);
    return TRUE;
}

/* concat rapporte la position DU FICHIER COURANT, pas un cumul */
static gdouble pos_global(void) {
    gint64 ns;
    if (!pipe_ || n_plan == 0) return -1;
    if (!gst_element_query_position(pipe_, GST_FORMAT_TIME, &ns)) return -1;
    return seg[plan[active]].start + (gdouble)ns / GST_SECOND;
}

/* ================== commandes ================== */

typedef struct { char line[256]; int fd; } Cmd;

static gboolean handle(gpointer data) {
    Cmd *c = (Cmd *)data;
    char *l = c->line;
    int fd = c->fd;
    l[strcspn(l, "\r\n")] = 0;

    if (!strncmp(l, "TIMELINE", 8)) {
        /* le client construit son affichage à partir de ça */
        GString *s = g_string_new("");
        g_string_append_printf(s, "TIMELINE %d %.3f", n_seg, total);
        for (int i = 0; i < n_seg; i++)
            g_string_append_printf(s, " %s:%.3f:%.3f",
                                   seg[i].file, seg[i].start, seg[i].duration);
        reply(fd, "%s", s->str);
        g_string_free(s, TRUE);

    } else if (!strncmp(l, "PLAY", 4)) {
        gboolean ok = play_from(0.0, 1.0);
        reply(fd, "%s PLAY", ok ? "OK" : "ERR");

    } else if (!strncmp(l, "REVERSEALL", 10)) {
        /* reverse complet : depuis la fin de la timeline */
        gboolean ok = play_from(total - 0.05, -1.0);
        reply(fd, "%s REVERSEALL depuis %.2f", ok ? "OK" : "ERR", total - 0.05);

    } else if (!strncmp(l, "REVERSE", 7)) {
        gdouble t;
        if (sscanf(l, "REVERSE %lf", &t) != 1) {
            t = pos_global();
            if (t < 0) t = total - 0.1;
        }
        gboolean ok = play_from(t, -1.0);
        reply(fd, "%s REVERSE %.2f", ok ? "OK" : "ERR", t);

    } else if (!strncmp(l, "FORWARD", 7)) {
        gdouble t;
        if (sscanf(l, "FORWARD %lf", &t) != 1) {
            t = pos_global();
            if (t < 0) t = 0;
        }
        gboolean ok = play_from(t, 1.0);
        reply(fd, "%s FORWARD %.2f", ok ? "OK" : "ERR", t);

    } else if (!strncmp(l, "STATUS", 6)) {
        gdouble g = pos_global();
        int i = seg_at(g);
        reply(fd, "POS %.2f | RATE %.1f | FICHIER %s | BRANCHE %d/%d",
              g, rate, i >= 0 ? seg[i].file : "?", active,
              n_plan > 0 ? n_plan - 1 : 0);

    } else if (!strncmp(l, "QUIT", 4)) {
        reply(fd, "BYE");
    } else if (*l) {
        reply(fd, "ERR commande inconnue");
    }

    g_free(c);
    return G_SOURCE_REMOVE;
}

static void *net_thread(void *arg) {
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
    g_print("%s[réseau]%s port %d\n\n", BLU, R, port);

    while (1) {
        int cfd = accept(sfd, NULL, NULL);
        if (cfd < 0) continue;
        char buf[512]; ssize_t n;
        while ((n = read(cfd, buf, sizeof(buf) - 1)) > 0) {
            buf[n] = 0;
            char *sp, *tok = strtok_r(buf, "\n", &sp);
            while (tok) {
                Cmd *c = g_new0(Cmd, 1);
                strncpy(c->line, tok, sizeof(c->line) - 1);
                c->fd = cfd;
                g_idle_add(handle, c);
                tok = strtok_r(NULL, "\n", &sp);
            }
        }
        close(cfd);
    }
    return NULL;
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);
    int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;

    g_print("\n%sconcat + seek par branche — N séquences%s\n\n", B, R);
    g_print("%sdécouverte des .mp4 du dossier...%s\n", D, R);
    discover();

    if (n_seg == 0) {
        g_print("%saucun .mp4 trouvé dans ce dossier%s\n", RED, R);
        return 1;
    }

    g_print("\n%s%d séquences, timeline de %.2fs :%s\n", B, n_seg, total, R);
    for (int i = 0; i < n_seg; i++)
        g_print("  %s%-12s%s %7.2fs -> %7.2fs   (%.2fs)\n",
                CYN, seg[i].file, R, seg[i].start,
                seg[i].start + seg[i].duration, seg[i].duration);
    g_print("\n");

    pthread_t tid;
    pthread_create(&tid, NULL, net_thread, &port);

    g_main_loop_run(g_main_loop_new(NULL, FALSE));
    return 0;
}
