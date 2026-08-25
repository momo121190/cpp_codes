#!/usr/bin/env python3
"""client.py — pilote le lecteur. Fonctionne avec n'importe quel nombre
de séquences : la timeline est récupérée depuis le serveur au démarrage."""

import socket
import sys
import time

try:
    import readline
except ImportError:
    pass

RESET = "\033[0m"; BOLD = "\033[1m"; DIM = "\033[2m"
RED = "\033[31m"; GRN = "\033[32m"; YEL = "\033[33m"; CYN = "\033[36m"

WIDTH = 54           # largeur de la barre de timeline
segments = []        # [(nom, start, duration), ...]
total = 0.0


def load_timeline(send):
    """Récupère la liste des séquences depuis le serveur."""
    global segments, total
    resp = send("TIMELINE")
    if not resp or not resp.startswith("TIMELINE"):
        return False
    parts = resp.split()
    # TIMELINE <n> <total> nom:start:dur nom:start:dur ...
    total = float(parts[2])
    segments = []
    for item in parts[3:]:
        name, start, dur = item.rsplit(":", 2)
        segments.append((name, float(start), float(dur)))
    return True


def bar(pos):
    """Barre ASCII avec une frontière par séquence."""
    b = ["."] * WIDTH
    for _, start, _d in segments[1:]:
        c = int((start / total) * WIDTH)
        if 0 < c < WIDTH:
            b[c] = "|"
    if pos is not None and pos >= 0:
        c = min(max(int((pos / total) * WIDTH), 0), WIDTH - 1)
        b[c] = "#"
    return "[" + "".join(b) + "]"


def seg_of(pos):
    for name, start, dur in segments:
        if start <= pos < start + dur:
            return name
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
    arrow = f"{YEL}<<{RESET}" if (rate or 0) < 0 else f"{GRN}>>{RESET}"
    extra = f"  {DIM}br {branch}{RESET}" if branch else ""
    print(f"  {bar(pos)} {CYN}{pos:6.2f}{RESET}/{total:.0f}s {arrow} "
          f"{DIM}{seg_of(pos)}{RESET}{extra}")


def print_timeline():
    print(f"\n{BOLD}{len(segments)} séquences — timeline de {total:.2f}s{RESET}")
    for name, start, dur in segments:
        print(f"  {CYN}{name:<12}{RESET} {start:7.2f}s -> {start+dur:7.2f}s"
              f"  {DIM}({dur:.2f}s){RESET}")
    print(f"""
{BOLD}Commandes{RESET}
  {CYN}p{RESET}          lecture depuis le début
  {CYN}r <t>{RESET}      reverse à partir de la seconde t
  {CYN}f <t>{RESET}      forward à partir de la seconde t
  {CYN}r{RESET} / {CYN}f{RESET}      depuis la position actuelle
  {CYN}ra{RESET}         reverse complet (toute la timeline)
  {CYN}s{RESET}          position actuelle
  {CYN}w [n]{RESET}      suivi en direct pendant n secondes (défaut 15)
  {CYN}t{RESET}          réaffiche la timeline
  {CYN}q{RESET}          quitter
""")


def main():
    host = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 5000

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    sock.settimeout(30.0)      # large : la construction peut être longue

    def send(cmd):
        sock.sendall((cmd + "\n").encode())
        try:
            return sock.recv(4096).decode().strip()
        except socket.timeout:
            return None

    print(f"{GRN}Connecté à {host}:{port}{RESET}")
    if not load_timeline(send):
        print(f"{RED}impossible de récupérer la timeline{RESET}")
        return
    print_timeline()

    try:
        while True:
            try:
                line = input(f"{BOLD}>> {RESET}").strip()
            except EOFError:
                break
            if not line:
                continue
            parts = line.split()
            cmd = parts[0].lower()

            if cmd in ("q", "quit", "exit"):
                send("QUIT"); break
            if cmd == "t":
                load_timeline(send); print_timeline(); continue

            if cmd == "w":
                secs = float(parts[1]) if len(parts) > 1 else 15.0
                print(f"{DIM}suivi {secs:.0f}s — Ctrl-C pour arrêter{RESET}")
                end = time.time() + secs
                try:
                    while time.time() < end:
                        r = send("STATUS")
                        if r:
                            show(*parse_status(r))
                        time.sleep(0.5)
                except KeyboardInterrupt:
                    print(f"\n{DIM}interrompu{RESET}")
                continue

            if cmd == "p":
                resp = send("PLAY")
            elif cmd == "s":
                resp = send("STATUS")
            elif cmd == "ra":
                resp = send("REVERSEALL")
            elif cmd == "r":
                resp = send(f"REVERSE {parts[1]}" if len(parts) > 1 else "REVERSE")
            elif cmd == "f":
                resp = send(f"FORWARD {parts[1]}" if len(parts) > 1 else "FORWARD")
            else:
                print(f"{RED}commande inconnue{RESET} — p r f s w t q")
                continue

            if resp is None:
                print(f"{YEL}(pas de réponse){RESET}")
                continue
            print(f"  {RED if resp.startswith('ERR') else DIM}{resp}{RESET}")

            if cmd in ("p", "r", "f", "ra"):
                time.sleep(0.5)
                st = send("STATUS")
                if st:
                    show(*parse_status(st))
    finally:
        sock.close()
        print(f"{DIM}Déconnecté.{RESET}")


if __name__ == "__main__":
    main()
