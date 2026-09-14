#!/usr/bin/env python3
# Quick look at mod4591Svec28.v: list its case blocks and assigns
# before hand-translating it to VHDL.

import re

src = open('mod4591Svec28.v').read()

# case blocks: always @(*) begin case(...) ... endcase end
cases = {}
for m in re.finditer(
        r"always\s*@\s*\(\s*\*\s*\)\s*begin\s*case\s*\(\s*\{([^}]+)\}\s*\)(.*?)endcase\s*end",
        src, re.S):
    sel = m.group(1)
    body = m.group(2)
    out = re.search(r"(\w+)\s*=", body).group(1)
    entries = re.findall(r"(\d+)'h([0-9a-fA-F]+)\s*:\s*\w+\s*=\s*(\d+)'d(\d+)", body)
    cases[out] = (sel.strip(), entries)

assigns = re.findall(r"assign\s+(\w+)\s*=\s*([^;]+);", src)

print("case blocks:", list(cases.keys()))
print("assigns:", [a[0] for a in assigns])
for out, (sel, ent) in cases.items():
    print(f"  {out}: sel={sel[:40]}..., {len(ent)} entries")
