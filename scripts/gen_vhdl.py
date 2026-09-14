import re

src = open('mod4591Svec28.v').read()

# Case bloklari
cases = {}
for m in re.finditer(r"always\s*@\s*\(\s*\*\s*\)\s*begin\s*case\s*\(\s*\{([^}]+)\}\s*\)(.*?)endcase\s*end", src, re.S):
    sel = m.group(1); body = m.group(2)
    out = re.search(r"(\w+)\s*=", body).group(1)
    bits = [b.strip() for b in sel.split(',')]
    ent = re.findall(r"(\d+)'h([0-9a-fA-F]+)\s*:\s*\w+\s*=\s*(\d+)'d(\d+)", body)
    cases[out] = (bits, ent)

# assign
asg = re.findall(r"assign\s+(\w+)\s*=\s*([^;]+);", src)

def vbit(b):
    m = re.match(r"z_in\[(\d+)\]", b)
    return f"z_in({m.group(1)})"

L = []
L.append("library ieee;")
L.append("use ieee.std_logic_1164.all;")
L.append("use ieee.numeric_std.all;")
L.append("")
L.append("entity mod4591Svec28 is")
L.append("    port (")
L.append("        z_in : in  std_logic_vector(27 downto 0);")
L.append("        p0   : out std_logic_vector(11 downto 0);")
L.append("        p1   : out std_logic_vector(11 downto 0);")
L.append("        n0   : out std_logic_vector(11 downto 0);")
L.append("        n1   : out std_logic_vector(11 downto 0)")
L.append("    );")
L.append("end entity mod4591Svec28;")
L.append("")
L.append("architecture rtl of mod4591Svec28 is")
for out,(bits,ent) in cases.items():
    L.append(f"    signal sel_{out} : std_logic_vector({len(bits)-1} downto 0);")
L.append("begin")
L.append("")
for out,(bits,ent) in cases.items():
    cat = " & ".join(vbit(b) for b in bits)
    L.append(f"    sel_{out} <= {cat};")
L.append("")
for out,(bits,ent) in cases.items():
    L.append(f"    -- {out}: {len(ent)} entries")
    L.append(f"    process(sel_{out})")
    L.append("    begin")
    L.append(f"        case sel_{out} is")
    for (w,h,ow,od) in ent:
        idx = int(h,16); nb = len(bits)
        binstr = format(idx, f'0{nb}b')
        L.append(f'            when "{binstr}" => {out} <= std_logic_vector(to_unsigned({od}, 12));')
    L.append(f'            when others => {out} <= (others => \'0\');')
    L.append("        end case;")
    L.append("    end process;")
    L.append("")
# p0 assign
for (name, expr) in asg:
    e = expr.strip()
    # z_in[11:0] gibi ifadeleri cevir
    e = re.sub(r"z_in\[(\d+):(\d+)\]", r"z_in(\1 downto \2)", e)
    e = re.sub(r"z_in\[(\d+)\]", r"z_in(\1)", e)
    e = re.sub(r"(\d+)'d(\d+)", r"std_logic_vector(to_unsigned(\2, \1))", e)
    e = re.sub(r"(\d+)'b([01]+)", r'"\2"', e)
    L.append(f"    -- assign {name} = {expr.strip()}")
    L.append(f"    {name} <= {e};")
L.append("")
L.append("end architecture rtl;")

open('mod4591Svec28.vhd','w').write("\n".join(L))
print("✓ mod4591Svec28.vhd uretildi")
print(f"  Satir: {len(L)}")
