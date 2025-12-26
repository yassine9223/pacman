#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Détecte et tente de décoder des séquences Base64 avec un alphabet personnalisé
# Usage: python3 try_decode.py path/to/binary

import sys
import re
import base64
from pathlib import Path
from string import printable

if len(sys.argv) != 2:
    print("Usage: python3 try_decode.py path/to/binary")
    sys.exit(1)

fpath = Path(sys.argv[1])
data = fpath.read_bytes()

# Si tu veux, tu peux remplacer par la sortie de `strings` (texte) :
try:
    text = data.decode('utf-8', errors='ignore')
except:
    text = ''.join(chr(b) if 32 <= b < 127 else '.' for b in data)

# alphabet observé dans ton dump (non standard) — on l'utilise ici
custom_alph = "ZYXABCDEFGHIJKLMNOPQRSTUVWzyxabcdefghijklmnopqrstuvw0123456789-_"
std_alph    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

# Construire map de custom -> standard
mapping = {c: std_alph[i] for i, c in enumerate(custom_alph)}

# regex pour trouver séquences longues composées des caractères possibles (+ padding = '=')
pattern = re.compile(r"[A-Za-z0-9\-_]{8,}=?=?")  # recherche heuristique

candidates = set(re.findall(pattern, text))
print(f"[+] Trouvé {len(candidates)} candidates base64-like à tester\n")

def map_custom_to_std(s):
    # remplace chaque char si présent dans mapping, sinon renvoie None
    out = []
    for ch in s:
        if ch == '=':
            out.append('=')
        elif ch in mapping:
            out.append(mapping[ch])
        else:
            # si caractère inconnu, retourne None pour indiquer échec du mapping
            return None
    return ''.join(out)

def looks_printable(b):
    try:
        s = b.decode('utf-8')
    except:
        return False
    # au moins 60% imprimable
    printable_ratio = sum(1 for c in s if c in printable) / max(1, len(s))
    return printable_ratio > 0.60 and any(ch.isalpha() for ch in s)

found = []
for cand in sorted(candidates, key=len, reverse=True):
    mapped = map_custom_to_std(cand)
    if mapped is None:
        continue
    # ajouter padding si nécessaire pour la longueur en base64
    mod = len(mapped) % 4
    if mod != 0:
        mapped += "=" * (4 - mod)
    try:
        decoded = base64.b64decode(mapped, validate=True)
    except Exception:
        continue
    if looks_printable(decoded) or b"CTF" in decoded or b"flag" in decoded.lower():
        found.append((cand, mapped, decoded))
        print("=== Candidate:", cand)
        print("Mapped -> standard base64:", mapped)
        try:
            print("Decoded (utf-8):", decoded.decode('utf-8', errors='replace'))
        except:
            print("Decoded (bytes):", decoded)
        print("-" * 60)

if not found:
    print("Aucune décodage convaincant détecté automatiquement.")
    print("Conseils :")
    print(" - Essaie d'extraire .rodata ou tableaux d'octets depuis le binaire (objdump/ghidra).")
    print(" - Si tu veux, upload le binaire ici et j'essaierai d'analyser plus finement.")
else:
    print(f"\n[+] {len(found)} décodage(s) plausibles trouvé(s).")
