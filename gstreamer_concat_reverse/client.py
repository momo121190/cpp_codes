#!/usr/bin/env python3
"""client.py — pilote le lecteur concat + seek par branche."""

import socket
import sys
import time

try:
    import readline
except ImportError:
    pass

RESET = "\033[0m"; BOLD = "\033[1m"; DIM = "\033[2m"
RED = "\033[31m"; GRN = "\033[32m"; YEL = "\033[33m"
BLU = "\033[34m"; CYN = "\033[36m"

SEGMENTS = [("SEQ 1", 0.0, 10.0), ("SEQ 2", 10.0, 8.0), ("SEQ 3", 18.0, 15.0)]
TOTAL = SEGMENTS[-1][1] + SEGMENTS[-1][2]

HELP = f"""
{BOLD}Commandes{RESET}
  {CYN}play{RESET}              lecture avant depuis 0
  {CYN}reverse <t>{RESET}       marche arrière à partir de la seconde t
  {CYN}forward <t>{RESET}       marche avant à partir de la seconde t
  {CYN}reverse{RESET}           marche arrière depuis la position actuelle
  {CYN}forward{RESET}           marche avant depuis la position actuelle
  {CYN}status{RESET}            position, sens, branche active
  {CYN}plan{RESET}              réaffiche le plan de lecture (côté serveur)
  {CYN}watch [n]{RESET}         suivi en direct pendant n secondes (défaut 12)
  {CYN}help{RESET} / {CYN}quit{RESET}

{BOLD}Timeline{RESET}   SEQ 1 : 0-10s  |  SEQ 2 : 10-18s  |  SEQ 3 : 18-33s

{DIM}Exemple du cas demandé : reverse à 5s dans SEQ 2  ->  'reverse 15'
(15 = 10 + 5, position globale). Le serveur affiche le plan :
SEQ 2 de 5s vers 0, puis SEQ 1 de 10s vers 0.{RESET}
"""


def bar(pos, width=46):
    b = ["."] * width
    for _, start, _d in SEGMENTS[1:]:
        c = int((start / TOTAL) * width)
        if 0 < c < width:
            b[c] = "|"
    if pos is not None and pos >= 0:
        c = min(max(int((pos / TOTAL) * width), 0), width - 1)
        b[c] = "#"
    return "[" + "".join(b) + "]"


def seg_of(pos):
    for label, start, dur in SEGMENTS:
        if start <= pos < start + dur:
            return label
    return "?"


def parse_status(txt):
    pos = rate = None
    branch = ""
    for part in txt.split("|"):
        part = part.strip()
        if part.startswith("POS "):
            try: pos = float(part[4:])
            except ValueError: pass
        elif part.startswith("RATE "):
            try: rate = float(part[5:])
            except ValueError: pass
        elif part.startswith("BRANCHE "):
            branch = part[8:]
    return pos, rate, branch


def show(pos, rate, branch=""):
    if pos is None or pos < 0:
        print(f"  {RED}position indisponible{RESET}")
        return
    arrow = f"{YEL}<<REV{RESET}" if (rate or 0) < 0 else f"{GRN}>>FWD{RESET}"
    extra = f"  {DIM}br {branch}{RESET}" if branch else ""
    print(f"  {bar(pos)} {CYN}{pos:6.2f}s{RESET}/{TOTAL:.0f}s  {arrow}  "
          f"{DIM}{seg_of(pos)}{RESET}{extra}")


def main():
    host = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 5000

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    sock.settimeout(8.0)
    print(f"{GRN}Connecté à {host}:{port}{RESET}")
    print(HELP)

    def send(cmd):
        sock.sendall((cmd + "\n").encode())
        try:
            return sock.recv(1024).decode().strip()
        except socket.timeout:
            return None

    try:
        while True:
            try:
                line = input(f"{BOLD}>> {RESET}").strip()
            except EOFError:
                break
            if not line:
                continue
            low = line.lower()
            parts = line.split()

            if low in ("quit", "exit"):
                send("QUIT"); break
            if low in ("help", "?"):
                print(HELP); continue

            if low.startswith("watch"):
                secs = 12.0
                if len(parts) == 2:
                    try: secs = float(parts[1])
                    except ValueError: pass
                print(f"{DIM}suivi {secs:.0f}s — Ctrl-C pour arrêter{RESET}")
                end = time.time() + secs
                try:
                    while time.time() < end:
                        r = send("STATUS")
                        if r:
                            p, rt, br = parse_status(r)
                            show(p, rt, br)
                        time.sleep(0.5)
                except KeyboardInterrupt:
                    print(f"\n{DIM}interrompu{RESET}")
                continue

            if low == "play":
                resp = send("PLAY")
            elif low == "plan":
                resp = send("PLAN")
            elif low == "status":
                resp = send("STATUS")
            elif low.startswith("reverse"):
                resp = send(f"REVERSE {parts[1]}" if len(parts) == 2 else "REVERSE")
            elif low.startswith("forward"):
                resp = send(f"FORWARD {parts[1]}" if len(parts) == 2 else "FORWARD")
            else:
                print(f"{RED}commande inconnue{RESET} (tape 'help')")
                continue

            if resp is None:
                print(f"{YEL}(pas de réponse){RESET}")
                continue
            print(f"  {RED if resp.startswith('ERR') else DIM}{resp}{RESET}")

            if low.startswith(("play", "reverse", "forward")):
                time.sleep(0.4)
                st = send("STATUS")
                if st:
                    p, rt, br = parse_status(st)
                    show(p, rt, br)
    finally:
        sock.close()
        print(f"{DIM}Déconnecté.{RESET}")


if __name__ == "__main__":
    main()
