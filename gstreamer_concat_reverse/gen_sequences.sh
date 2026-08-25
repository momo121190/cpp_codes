#!/bin/bash
# Génère 3 séquences visuellement distinctes avec timecode incrusté.
set -e

W=640; H=480; FPS=30; GOP=15   # GOP court = reverse plus fluide

gen() {
    local out=$1 pattern=$2 label=$3 nbuf=$4
    echo "  -> $out ($((nbuf/FPS))s)"
    gst-launch-1.0 -q videotestsrc num-buffers=$nbuf $pattern ! \
      video/x-raw,width=$W,height=$H,framerate=$FPS/1 ! \
      textoverlay text="$label" valignment=top halignment=left \
        font-desc="Sans Bold 36" shaded-background=true ! \
      timeoverlay halignment=center valignment=center \
        font-desc="Sans 44" shaded-background=true ! \
      x264enc key-int-max=$GOP tune=zerolatency ! \
      mp4mux faststart=true ! filesink location=$out
}

echo "== Génération =="
gen seq1.mp4 "pattern=ball background-color=0xFF1144AA" "SEQ 1 (10s)" 300
gen seq2.mp4 "pattern=checkers-8"                        "SEQ 2 (8s)"  240
gen seq3.mp4 "pattern=smpte"                             "SEQ 3 (15s)" 450

echo
echo "== Timeline globale =="
echo "  SEQ 1 :  0.0s -> 10.0s   (bleu, balle)"
echo "  SEQ 2 : 10.0s -> 18.0s   (damier)"
echo "  SEQ 3 : 18.0s -> 33.0s   (barres SMPTE)"
