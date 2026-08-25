# reverse / forward à un instant donné — N séquences

Le serveur découvre les `.mp4` du dossier au démarrage, lit leurs durées
et construit la timeline globale. Le client récupère cette timeline par
le réseau : rien n'est codé en dur, ni côté serveur ni côté client.

## Lancement

```bash
chmod +x gen.sh
./gen.sh 9              # 9 séquences de test (ou le nombre voulu)
make
./server                # terminal 1
python3 client.py       # terminal 2
```

Si le port est occupé : `fuser -k 5000/tcp`

## Commandes du client

```
p          lecture depuis le début
r 45       reverse à partir de la seconde 45
f 45       forward à partir de la seconde 45
r  /  f    depuis la position actuelle
s          position
w 20       suivi en direct pendant 20s
t          réaffiche la timeline
q          quitter
```

## Le test avec 9 séquences

```
>> r 45
```

Le serveur affiche le nombre de branches construites, puis un seek par
branche :

```
REVERSE à 45.00s = seq06.mp4 + 3.00s — 6 branche(s)
seeks :
  d0  seq06.mp4    rate=-1 borne=  3.00s  ok
  d1  seq05.mp4    rate=-1 borne= 14.00s  ok
  d2  seq04.mp4    rate=-1 borne=  8.00s  ok
  d3  seq03.mp4    rate=-1 borne= 14.00s  ok
  d4  seq02.mp4    rate=-1 borne= 11.00s  ok
  d5  seq01.mp4    rate=-1 borne=  8.00s  ok
```

Puis, pendant la lecture, une ligne par franchissement :

```
[bascule] branche 1/5 -> seq05.mp4
  [re-seek] d1  seq05.mp4  rate=-1 borne= 14.00s  ok
```

Suis la position :

```
>> w 40
```

### Ce qu'il faut vérifier

1. **Tous les seeks affichent `ok`.** Un seul `REFUSE` et l'enchaînement
   cassera à ce point.
2. **Les bascules sont numérotées sans trou** : 1, 2, 3... jusqu'à
   `n_plan - 1`.
3. **La position décroît continûment** pendant un reverse, y compris au
   passage d'une séquence à l'autre.

## Notes d'implémentation

- `h264parse` est placé **par branche** (avant `concat`) et non après :
  avec beaucoup de séquences, les SPS/PPS peuvent différer d'un fichier
  à l'autre, et le flux est ainsi normalisé avant d'entrer dans `concat`.
- Le timeout de passage en `PAUSED` est à 20s : avec 9 branches, tous les
  `qtdemux` doivent analyser leur fichier avant que l'état soit atteint.
- `MAX_SEG` est à 64.

## Fonctions à reprendre

| Fonction | Rôle |
|---|---|
| `discover()` | construit la table des segments — à remplacer par tes métadonnées réelles |
| `play_from(t, rate)` | tout le mécanisme |
| `seek_branch(b)` | le seek d'une branche |
| `on_pad()` | rejeu du seek à la bascule + suivi de la branche active |
| `pos_global()` | position globale = offset du segment actif + position locale |
