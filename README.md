# ASIC Synthesis Artifact: Five-Way Characteristic-Three Multiplication for NTRU Prime

This artifact reproduces the ASIC synthesis results for three sntrup761
R/3 multipliers and their decapsulation-core integration, on the Nangate 45 nm
open cell library.

## Designs

Three multipliers, each integrated into the same sntrup761 decapsulation:

| Design | R/3 method | Top module |
|---|---|---|
| U1 | Yeniaras-Cenk 5-way split (this work) | key_decap_wrapper_u1 |
| B1 | Bernstein 3-way Karatsuba | key_decap_wrapper_b1 |
| Peng | parallel schoolbook | key_decap_wrapper |

## Layout

```
src/vhdl/    VHDL sources (decapsulation, memories, freeze, encoding wrappers)
data/          reference-C operation-count harness (U1/B1 by Yeniaras-Cenk; I/O from Peng) + KAT inputs
src/testbench/ Verilog testbenches
src/vlog/    Verilog sources (U1/B1 multipliers, bridges, Peng encoding)
src/nangate45.lib   standard-cell library
scripts/     Yosys synthesis scripts and the VHDL-to-Verilog helper scripts
reports/     synthesis logs with chip area (see the mapping below)
LICENSE_PENG_SNTRUP_FPGA   license of the Peng et al. base design
```

## Synthesis logs (reports/)

Each area figure in the paper is the `chip area` line of one of these logs. The paper's
Appendix (Synthesis reproduction) lists the same mapping.

| Log file | What it synthesizes | Cell area (um^2) | Paper |
|---|---|---|---|
| `synth_mult_u1.log`    | U1 multiplier, deployable (single engine)   | 431,430   | 0.43 mm^2 |
| `synth_mult_b1.log`    | B1 multiplier, serial                        | 902,373   | 0.90 mm^2 |
| `synth_mult_peng.log`  | Peng parallel-schoolbook multiplier          | 320,057   | 0.32 mm^2 |
| `synth_comb_u1.log`    | U1 fully combinational                       | 2,705,068 | 2.71 mm^2 |
| `synth_comb_b1.log`    | B1 fully combinational                       | 2,446,018 | 2.45 mm^2 |
| `synth_decap_u1.log`   | U1 decapsulation core             | 972,392   | 0.97 mm^2 |
| `synth_decap_b1.log`   | B1 decapsulation core             | 1,443,994 | 1.44 mm^2 |
| `synth_decap_peng.log` | Peng decapsulation wrapper (multiplier at top level, added separately) | 479,108 | 0.48 mm^2 |

Peng decapsulation core in the paper is 0.80 mm^2 = 0.48 (wrapper) + 0.32 (shared
multiplier), since Peng instantiates its multiplier at the top level rather than inside
the decapsulation.

## Tools

- Yosys 0.33 (with ABC)
- GHDL (for the VHDL-to-Verilog step), any recent version with --std=08

## Flow

Yosys does not read VHDL, so the decapsulation VHDL is first translated to Verilog
with GHDL, then synthesized together with the real Verilog multipliers.

1. Analyze the VHDL with GHDL and emit Verilog:
   ```
   ghdl -a --std=08 <all vhdl in dependency order>
   ghdl synth --std=08 --out=verilog key_decap_wrapper_u1 > decap_u1.v
   ```
   The Verilog modules that the VHDL instantiates (decode_rp, encode_rp, the
   rp761 parameter modules) are declared as empty blackbox entities in
   verilog_blackboxes.vhd so GHDL can elaborate. These blackboxes are removed from
   the GHDL output; Yosys links the real Verilog instead.

2. Two Verilog lookup modules from Peng's design (modmul4591S, mod4591Svec28) are
   hand-translated to VHDL (in scripts/) so GHDL can resolve modq_freeze.

3. Synthesize with Yosys:
   ```
   yosys -s scripts/synth_decap_u1.ys
   ```
   The script reads the cleaned GHDL output plus all real Verilog, maps to
   Nangate 45 nm, and prints the chip area.

Repeat for B1 (synth_decap_b1.ys) and Peng (synth_decap_peng.ys).

## Results (decapsulation core, Nangate 45 nm)

| Design | Area (mm^2) | Cycles | ADP (mm^2 x cyc) |
|---|---|---|---|
| Peng | 0.80 | 3829 | 3060 |
| U1 | 0.97 | 2471 | 2403 |
| B1 | 1.44 | 2720 | 3928 |

U1 has the lowest area-delay product: 1.27x better than Peng, 1.63x better than B1.

Peng instantiates its multiplier once at a shared top level, outside the
decapsulation, so its decap-only wrapper (0.48 mm^2) excludes it. The 0.80 mm^2
figure adds the standalone multiplier (0.32 mm^2), so all three cover the same
function. U1 and B1 embed their multipliers through a bridge and need no adjustment.

The synthesis logs in reports/ contain the exact chip-area figures.
