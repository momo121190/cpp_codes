#!/bin/bash
# Génère N séquences de test. Usage : ./gen.sh [nombre]   (défaut 9)
set -e
N=${1:-9}
FPS=30

rm -f seq*.mp4
echo "== Génération de $N séquences =="

for i in $(seq 1 $N); do
    dur=$(( 5 + (i * 3) % 12 ))          # durées variées : 5 à 16s
    nbuf=$(( dur * FPS ))
    case $((i % 3)) in
      1) pat="pattern=ball background-color=0xFF1144AA" ;;
      2) pat="pattern=checkers-8" ;;
      0) pat="pattern=smpte" ;;
    esac
    printf "  seq%02d.mp4  %2ds\n" $i $dur
    gst-launch-1.0 -q videotestsrc num-buffers=$nbuf $pat ! \
      video/x-raw,width=640,height=480,framerate=$FPS/1 ! \
      textoverlay text="SEQ $i" valignment=top halignment=left \
        font-desc="Sans Bold 40" shaded-background=true ! \
      timeoverlay halignment=center valignment=center \
        font-desc="Sans 44" shaded-background=true ! \
      x264enc key-int-max=15 tune=zerolatency ! \
      mp4mux faststart=true ! filesink location=$(printf "seq%02d.mp4" $i)
done

echo
echo "OK — $N séquences générées."
echo "Le serveur les découvre tout seul au démarrage : ./server"
