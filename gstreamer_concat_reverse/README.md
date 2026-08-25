# concat + seek par branche

Reverse et forward à partir d'un instant donné, à travers plusieurs
séquences, **en n'utilisant que `concat`**.

## Le principe

La lecture du code source de `concat` (`gstconcat.c`) donne trois faits
qui expliquent pourquoi un seek global ne marche pas :

| Fonction | Ce qu'elle fait | Conséquence |
|---|---|---|
| `gst_concat_src_event()` | pousse le SEEK vers `current_sinkpad` seulement | les autres branches ne le reçoivent jamais |
| `gst_concat_src_query()` | ne traite pas `DURATION` | concat rapporte la durée d'un seul segment |
| `gst_concat_switch_pad()` | ne change de branche que sur EOS, dans l'ordre des pads | pas de retour arrière possible entre segments |

Détail utile : dans `gst_concat_request_new_pad()`, le nom du pad est
toujours regénéré (`sink_%u` avec un compteur interne) — écrire
`c.sink_0` explicitement ne change donc rien, le paramètre `name` est
ignoré. Seul l'**ordre de linkage** compte.

**La solution ne demande rien de tout ça à concat.** À la place :

1. On construit le pipeline avec uniquement les séquences à lire,
   **dans l'ordre de lecture voulu** (inversé pour un reverse).
2. On envoie un seek **à chaque branche séparément**, sur son `qtdemux`
   nommé, pendant que le pipeline est en `PAUSED`.
3. La première branche est bornée à l'instant demandé, les suivantes
   sont intégrales.
4. `concat` enchaîne les branches sur EOS — son fonctionnement normal,
   celui qui marche déjà chez toi.

### Exemple : reverse à 5s dans SEQ 2

```
branche 0 : seq2.mp4   rate=-1   start=0   stop=5s
branche 1 : seq1.mp4   rate=-1   start=0   stop=10s
```

SEQ 2 joue de 5s vers 0 → EOS → concat bascule → SEQ 1 joue de 10s vers
0 → EOS final.

## Installation et lancement

```bash
chmod +x gen_sequences.sh
./gen_sequences.sh
make
./player                 # terminal 1
python3 client.py        # terminal 2
```

Si le port est pris : `fuser -k 5000/tcp`.

## Le test du cas demandé

```
>> reverse 15
```

15 = 10 (début de SEQ 2) + 5 → c'est exactement « reverse à 5s de SEQ 2 ».

Le serveur affiche alors le plan avant de lancer :

```
┌─ PLAN DE LECTURE ─────────────────────────────────────
│ sens : REVERSE   branches : 2
│  [0] SEQ 2  seq2.mp4    joue 5.00s -> 0.00s (span 5.00s)  d0
│  [1] SEQ 1  seq1.mp4    joue 10.00s -> 0.00s (span 10.00s)  d1
└───────────────────────────────────────────────────────

┌─ SEEK PAR BRANCHE ────────────────────────────────────
│ [0] d0   SEQ 2   rate=-1.0  start=0.00s  stop=5.00s   OK
│ [1] d1   SEQ 1   rate=-1.0  start=0.00s  stop=10.00s  OK
└───────────────────────────────────────────────────────
```

Puis, pendant la lecture, chaque changement de segment est tracé :

```
[bascule] concat -> branche 1  (SEQ 1, seq1.mp4)
```

Suis la position en direct :

```
>> watch 20
```

La position doit descendre de 15 vers 10 (fin de SEQ 2), puis de 10 vers
0 (SEQ 1), avec l'image qui passe du damier à la balle bleue.

### Autres tests

```
>> reverse 25      # depuis SEQ 3 : doit traverser SEQ 3, SEQ 2, SEQ 1
>> forward 15      # depuis 5s de SEQ 2 vers l'avant : SEQ 2 puis SEQ 3
>> play            # lecture normale complète
>> plan            # réaffiche le plan courant
```

## Ce qui reste à vérifier chez toi

Le point que je n'ai pas pu tester : `concat` bloque les branches non
courantes (`gst_concat_pad_wait`). Les seeks sont donc envoyés en
`PAUSED`, avant que les branches soient débloquées, pour que chaque
segment soit configuré d'avance.

Si une branche ignore son seek et repart en avant, le filet de sécurité
est déjà en place : le callback `on_active_pad()` est notifié à chaque
bascule. Il suffit d'y réappliquer le seek sur la branche qui vient de
devenir courante (le code affiche déjà la bascule, il n'y a qu'à ajouter
le `gst_element_seek` correspondant).

Regarde dans la sortie si les lignes `SEEK PAR BRANCHE` affichent toutes
`OK` — et surtout, pendant le `watch`, si la position continue de
descendre après la ligne `[bascule]`.

## Transposition vers ton service replay

- `segments[]` est en dur pour le test. Chez toi, à construire
  dynamiquement à partir des durées réelles des séquences enregistrées.
- `build_and_start(seg_index, local_t, rate)` est la fonction à
  reprendre : elle prend la séquence, l'instant local, le sens, et fait
  tout le reste.
- `MAX_BRANCH` est à 16, largeur suffisante pour tes 2 à 9 séquences.
- Le pipeline est reconstruit à chaque changement de sens. Si ton flux
  ne le permet pas, la variante est de préparer les deux pipelines à
  l'avance et de basculer le sink de l'un à l'autre.
