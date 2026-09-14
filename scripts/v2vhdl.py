import re, sys

f = open('mod4591Svec28.v').read()

# Case bloklarini bul
cases = {}
for m in re.finditer(r"always\s*@\s*\(\s*\*\s*\)\s*begin\s*case\s*\(\s*\{([^}]+)\}\s*\)(.*?)endcase\s*end", f, re.S):
    sel_bits = m.group(1)
    body = m.group(2)
    # Cikis adini bul
    out = re.search(r"(\w+)\s*=", body).group(1)
    entries = re.findall(r"(\d+)'h([0-9a-fA-F]+)\s*:\s*\w+\s*=\s*(\d+)'d(\d+)", body)
    cases[out] = (sel_bits.strip(), entries)

# assign'lari bul
assigns = re.findall(r"assign\s+(\w+)\s*=\s*([^;]+);", f)

print(f"Bulunan case bloklari: {list(cases.keys())}")
print(f"Bulunan assign'lar: {[a[0] for a in assigns]}")
for out, (sel, ent) in cases.items():
    print(f"  {out}: sel={sel[:40]}..., {len(ent)} entry")
