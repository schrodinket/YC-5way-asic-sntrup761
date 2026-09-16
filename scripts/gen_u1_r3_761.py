#!/usr/bin/env python3
# u1_r3_761: full R/3 multiplier = ue_v14_real (product) + reduction
# Input: two degree-761 real polynomials (ar, br)
# Output: degree-761 R/3 result (mod x^761-x-1, mod 3)
# Reduction: single combinational layer (Peng uses 761 cycles, this uses one)
#   out[k] = ham[k] + ham[k+760] (k in 1..760) + ham[k+761] (k in 0..759)
# Python PASS (5000). f3_add ile combinational.
P=761

def gen():
    L=[];decl=[];Z="2'd0"
    L.append("// u1_r3_761: full R/3 multiplier (product + reduction), combinational")
    L.append("// ue_v14_real (1521 coeffs) + single-layer reduction (761)")
    L.append("// Reduction: x^761 = x+1, no chaining (a single layer suffices)")
    L.append("module u1_r3_761 (")
    L.append(f"    input  [{2*P-1}:0] ar, input [{2*P-1}:0] br,")
    L.append(f"    output [{2*P-1}:0] cr")
    L.append(");")
    # raw product: ue_v14_real output has 1521 coeffs (2*761-1)
    L.append(f"    wire [{2*(2*P-1)-1}:0] ham;")
    L.append(f"    ue_v14_real mult(.ar(ar),.br(br),.cr(ham));")
    # ham[i] = ham[2*i+1:2*i]
    def h(i): return f"ham[{2*i+1}:{2*i}]"
    # her ctwos coeffsi out[k]
    for k in range(P):
        terms=[h(k)]  # ham[k]
        if 1<=k<=760: terms.append(h(k+P-1))   # ham[k+760]
        if k+P<=2*P-2: terms.append(h(k+P))     # ham[k+761]
        if len(terms)==1:
            decl.append(f"    wire [1:0] O{k}={terms[0]};")
        else:
            cur=terms[0]
            for t,tm in enumerate(terms[1:]):
                nv=f"O{k}" if t==len(terms)-2 else f"O{k}_{t}"
                decl.append(f"    wire [1:0] {nv}; f3_add ar{k}_{t}(.a({cur}),.b({tm}),.c({nv}));")
                cur=nv
    L+=decl
    L.append(f"    assign cr={{{','.join(f'O{k}' for k in range(P-1,-1,-1))}}};")
    L.append("endmodule")
    return "\n".join(L)

if __name__=="__main__":
    code=gen()
    open("u1_r3_761_0603.v","w").write(code+"\n")
    print(f"u1_r3_761_0603.v uretildi ({len(code.splitlines())} satir)")
    # kac f3_add reduction icin
    print(f"reduction f3_add sayisi: ~{code.count('f3_add')} (cok az - tek layer)")
