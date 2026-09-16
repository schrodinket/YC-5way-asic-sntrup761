#!/usr/bin/env python3
"""
compute_latency.py
U1-hybrid vs B1 ASIC latency for sntrup761.

Inputs:
  - measured base-cell delays (Nangate 45nm, Yosys + ABC stime)
  - exact operation counts from the instrumented C code (see instrument.py)

Output: latency and U1's speed advantage for four design scenarios.

Run with:  python3 compute_latency.py
All constants are below, so you can change them and re-run.
"""

# ----------------------------------------------------------------------
# 1. Measured base-cell delays (Nangate 45nm, ABC stime, flattened)
#    Source: output of measure_timing.sh
# ----------------------------------------------------------------------
T_F3MUL = 57.95e-3   # ns, one F_3 multiply
T_F3ADD = 89.83e-3   # ns, one F_3 add
T_F9MUL = 138.27e-3  # ns, one F_9 multiply (four F_3 multiplies in parallel)

# Measured schoolbook block delays (combinational, single cycle), in ps
SB16R_PS = 2740.83   # real schoolbook, n=16
SB17R_PS = 2929.52   # real schoolbook, n=17
SB16C_PS = 3104.92   # complex schoolbook, n=16

# ----------------------------------------------------------------------
# 2. Exact operation counts (from instrumented C, checked by static analysis)
#    schoolbook size distribution
# ----------------------------------------------------------------------
U1_REAL = {14: 3, 15: 16, 16: 8}              # real schoolbook calls
U1_CPLX = {6: 27, 14: 3, 15: 36, 16: 60}      # complex (F_9) schoolbook calls
B1_REAL = {16: 381, 17: 24}                   # B1 is all real

# F_3 adds and subtracts in recombination
U1_ADD = 63851
B1_ADD = 58654

# ----------------------------------------------------------------------
# 3. Derived counts
# ----------------------------------------------------------------------
def mult_count(dist):
    """A schoolbook of size n does n*n coefficient products."""
    return sum(c * n * n for n, c in dist.items())

U1_MUL_R = mult_count(U1_REAL)   # real products
U1_MUL_C = mult_count(U1_CPLX)   # complex (F_9) products
B1_MUL   = mult_count(B1_REAL)   # B1 products

# Estimate block delay for sizes 6, 14, 15 by a line through the n=16 and n=17 points
_slope = (SB17R_PS - SB16R_PS) / (17 - 16)
_base  = SB16R_PS - _slope * 16
def delay_real_ps(n): return _slope * n + _base
def delay_cplx_ps(n): return (_slope * n + _base) * (SB16C_PS / SB16R_PS)

# ----------------------------------------------------------------------
# 4. Scenarios
# ----------------------------------------------------------------------
def to_us(ns): return ns / 1000.0

def scenario_S1_serial():
    """One shared multiplier and adder, every operation in turn. Lower bound."""
    u1 = to_us(U1_MUL_R*T_F3MUL + U1_MUL_C*T_F9MUL + U1_ADD*T_F3ADD)
    b1 = to_us(B1_MUL*T_F3MUL + B1_ADD*T_F3ADD)
    return u1, b1

def scenario_S2_parallel_recomb():
    """Serial multiply, recombination as a parallel adder tree (log depth)."""
    u1_rp = 5 * 4 * T_F3ADD   # 5 levels, 4 add layers on the critical path
    b1_rp = 6 * 3 * T_F3ADD
    u1 = to_us(U1_MUL_R*T_F3MUL + U1_MUL_C*T_F9MUL + u1_rp)
    b1 = to_us(B1_MUL*T_F3MUL + b1_rp)
    return u1, b1

def scenario_S4_pure_mult():
    """Multiply layer only. Minimal assumption about the rest of the design."""
    u1 = to_us(U1_MUL_R*T_F3MUL + U1_MUL_C*T_F9MUL)
    b1 = to_us(B1_MUL*T_F3MUL)
    return u1, b1

def scenario_S3_block_parallel():
    """Each schoolbook is one combinational block, recombination hidden. Upper bound."""
    u1 = (sum(c*delay_real_ps(n) for n,c in U1_REAL.items())
          + sum(c*delay_cplx_ps(n) for n,c in U1_CPLX.items())) / 1e6
    b1 = (B1_REAL[16]*delay_real_ps(16) + B1_REAL[17]*delay_real_ps(17)) / 1e6
    return u1, b1

# ----------------------------------------------------------------------
# 5. Report
# ----------------------------------------------------------------------
def advantage(u1, b1): return 100.0 * (1 - u1 / b1)

if __name__ == "__main__":
    print("="*64)
    print("  U1-hybrid vs B1  ASIC latency (sntrup761, n=761)")
    print("  Nangate 45nm, measured delays + exact operation counts")
    print("="*64)
    print(f"\nCalls: U1 = {sum(U1_REAL.values())+sum(U1_CPLX.values())} schoolbook "
          f"(real {sum(U1_REAL.values())} + complex {sum(U1_CPLX.values())})")
    print(f"       B1 = {sum(B1_REAL.values())} schoolbook (all real)")
    print(f"Products: U1 = {U1_MUL_R+U1_MUL_C:,} (real {U1_MUL_R:,} + complex {U1_MUL_C:,})")
    print(f"          B1 = {B1_MUL:,}")
    print(f"Recombination F_3 adds: U1 = {U1_ADD:,} | B1 = {B1_ADD:,}")
    print()
    print(f"{'Scenario':<28}{'U1 (us)':>10}{'B1 (us)':>10}{'Advantage':>11}")
    print("-"*59)
    for name, fn in [("S1 serial (lower bound)", scenario_S1_serial),
                     ("S2 parallel recomb.",     scenario_S2_parallel_recomb),
                     ("S4 pure multiply",         scenario_S4_pure_mult),
                     ("S3 block-parallel (upper)",scenario_S3_block_parallel)]:
        u1, b1 = fn()
        print(f"{name:<28}{u1:>10.3f}{b1:>10.3f}{advantage(u1,b1):>10.1f}%")
    print()
    print("Reference: C on Intel i7, sntrup761 multiply speedup = 35.52% (Yeniaras-Cenk 2021)")
    print("ASIC S4 (pure multiply) = 36.9%, which matches across platforms.")


# ----------------------------------------------------------------------
# sntrup653 (second parameter set)
# Exact inventory from data/hyb_1_u1.c (n=653) and data/hyb_1_b1.c (n=654).
# The sntrup653 tree passes through an A3 step, so it is addition-heavy and
# the matching scenario is S1 (full latency), not S4.
# ----------------------------------------------------------------------
def report_653():
    u1_real_653 = {12:1, 13:8, 23:2, 27:4}
    u1_cplx_653 = {9:150, 11:3, 12:6, 13:9}
    b1_real_653 = {13:69, 14:336}
    u1_add_653 = 38808
    b1_add_653 = 41112
    u1_mr = mult_count(u1_real_653); u1_mc = mult_count(u1_cplx_653)
    b1_m  = mult_count(b1_real_653)
    u1_s1 = to_us(u1_mr*T_F3MUL + u1_mc*T_F9MUL + u1_add_653*T_F3ADD)
    b1_s1 = to_us(b1_m*T_F3MUL + b1_add_653*T_F3ADD)
    print()
    print("="*64)
    print("  sntrup653 (n=653) full latency S1")
    print("="*64)
    print(f"U1: products {u1_mr+u1_mc:,}, additions {u1_add_653:,}")
    print(f"B1: products {b1_m:,}, additions {b1_add_653:,}")
    print(f"S1 full: U1 = {u1_s1:.3f} us, B1 = {b1_s1:.3f} us, "
          f"advantage = {advantage(u1_s1,b1_s1):.1f}%")
    print("Reference: software sntrup653 advantage = 29.85% (Yeniaras-Cenk)")

if __name__ == "__main__":
    report_653()
