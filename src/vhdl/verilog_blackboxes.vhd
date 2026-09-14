-- Stubs for the Verilog modules called from the Peng VHDL, so GHDL can elaborate.
-- Yosys links the real Verilog. Widths follow params.v.

library ieee;
use ieee.std_logic_1164.all;

-- ---- decode_rp -----------------------------------------------------------
entity decode_rp is
    port (
        clk             : in  std_logic;
        start           : in  std_logic;
        done            : out std_logic;
        rp_rd_addr      : out std_logic_vector(10 downto 0);
        rp_rd_data      : in  std_logic_vector(7 downto 0);
        cd_wr_addr      : out std_logic_vector(9 downto 0);
        cd_wr_data      : out std_logic_vector(13 downto 0);
        cd_wr_en        : out std_logic;
        state_l         : out std_logic_vector(4 downto 0);
        state_e         : out std_logic_vector(4 downto 0);
        state_s         : out std_logic_vector(4 downto 0);
        state_max       : in  std_logic_vector(4 downto 0);
        param_r_max     : in  std_logic_vector(8 downto 0);
        param_ro_max    : in  std_logic_vector(9 downto 0);
        param_small_r2  : in  std_logic;
        param_state_ct  : in  std_logic_vector(10 downto 0);
        param_ri_offset : in  std_logic_vector(8 downto 0);
        param_ri_len    : in  std_logic_vector(8 downto 0);
        param_outoffset : in  std_logic_vector(10 downto 0);
        param_outs1     : in  std_logic_vector(1 downto 0);
        param_outsl     : in  std_logic_vector(1 downto 0);
        param_m0        : in  std_logic_vector(13 downto 0);
        param_m0inv     : in  std_logic_vector(26 downto 0);
        param_ro_offset : in  std_logic_vector(8 downto 0)
    );
end entity decode_rp;
architecture blackbox of decode_rp is
begin
    done <= '0'; rp_rd_addr <= (others=>'0'); cd_wr_addr <= (others=>'0');
    cd_wr_data <= (others=>'0'); cd_wr_en <= '0';
    state_l <= (others=>'0'); state_e <= (others=>'0'); state_s <= (others=>'0');
end architecture blackbox;

-- ---- encode_rp -----------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
entity encode_rp is
    port (
        clk             : in  std_logic;
        start           : in  std_logic;
        done            : out std_logic;
        state_l         : out std_logic_vector(4 downto 0);
        state_s         : out std_logic_vector(4 downto 0);
        rp_rd_addr      : out std_logic_vector(9 downto 0);
        rp_rd_data      : in  std_logic_vector(13 downto 0);
        cd_wr_addr      : out std_logic_vector(10 downto 0);
        cd_wr_data      : out std_logic_vector(7 downto 0);
        cd_wr_en        : out std_logic;
        state_max       : in  std_logic_vector(4 downto 0);
        param_r_max     : in  std_logic_vector(9 downto 0);
        param_m0        : in  std_logic_vector(13 downto 0);
        param_1st_round : in  std_logic;
        param_outs1     : in  std_logic_vector(2 downto 0);
        param_outsl     : in  std_logic_vector(2 downto 0)
    );
end entity encode_rp;
architecture blackbox of encode_rp is
begin
    done <= '0'; state_l <= (others=>'0'); state_s <= (others=>'0');
    rp_rd_addr <= (others=>'0'); cd_wr_addr <= (others=>'0');
    cd_wr_data <= (others=>'0'); cd_wr_en <= '0';
end architecture blackbox;

-- ---- rp761q4591decode_param ---------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
entity rp761q4591decode_param is
    port (
        state_l         : in  std_logic_vector(4 downto 0);
        state_e         : in  std_logic_vector(4 downto 0);
        state_s         : in  std_logic_vector(4 downto 0);
        state_max       : out std_logic_vector(4 downto 0);
        param_r_max     : out std_logic_vector(8 downto 0);
        param_ro_max    : out std_logic_vector(9 downto 0);
        param_small_r2  : out std_logic;
        param_state_ct  : out std_logic_vector(10 downto 0);
        param_ri_offset : out std_logic_vector(8 downto 0);
        param_ri_len    : out std_logic_vector(8 downto 0);
        param_outoffset : out std_logic_vector(10 downto 0);
        param_outs1     : out std_logic_vector(1 downto 0);
        param_outsl     : out std_logic_vector(1 downto 0);
        param_m0        : out std_logic_vector(13 downto 0);
        param_m0inv     : out std_logic_vector(26 downto 0);
        param_ro_offset : out std_logic_vector(8 downto 0)
    );
end entity rp761q4591decode_param;
architecture blackbox of rp761q4591decode_param is
begin
    state_max <= (others=>'0'); param_r_max <= (others=>'0');
    param_ro_max <= (others=>'0'); param_small_r2 <= '0';
    param_state_ct <= (others=>'0'); param_ri_offset <= (others=>'0');
    param_ri_len <= (others=>'0'); param_outoffset <= (others=>'0');
    param_outs1 <= (others=>'0'); param_outsl <= (others=>'0');
    param_m0 <= (others=>'0'); param_m0inv <= (others=>'0');
    param_ro_offset <= (others=>'0');
end architecture blackbox;

-- ---- rp761q1531decode_param (same ports as q4591) ------------------------
library ieee;
use ieee.std_logic_1164.all;
entity rp761q1531decode_param is
    port (
        state_l         : in  std_logic_vector(4 downto 0);
        state_e         : in  std_logic_vector(4 downto 0);
        state_s         : in  std_logic_vector(4 downto 0);
        state_max       : out std_logic_vector(4 downto 0);
        param_r_max     : out std_logic_vector(8 downto 0);
        param_ro_max    : out std_logic_vector(9 downto 0);
        param_small_r2  : out std_logic;
        param_state_ct  : out std_logic_vector(10 downto 0);
        param_ri_offset : out std_logic_vector(8 downto 0);
        param_ri_len    : out std_logic_vector(8 downto 0);
        param_outoffset : out std_logic_vector(10 downto 0);
        param_outs1     : out std_logic_vector(1 downto 0);
        param_outsl     : out std_logic_vector(1 downto 0);
        param_m0        : out std_logic_vector(13 downto 0);
        param_m0inv     : out std_logic_vector(26 downto 0);
        param_ro_offset : out std_logic_vector(8 downto 0)
    );
end entity rp761q1531decode_param;
architecture blackbox of rp761q1531decode_param is
begin
    state_max <= (others=>'0'); param_r_max <= (others=>'0');
    param_ro_max <= (others=>'0'); param_small_r2 <= '0';
    param_state_ct <= (others=>'0'); param_ri_offset <= (others=>'0');
    param_ri_len <= (others=>'0'); param_outoffset <= (others=>'0');
    param_outs1 <= (others=>'0'); param_outsl <= (others=>'0');
    param_m0 <= (others=>'0'); param_m0inv <= (others=>'0');
    param_ro_offset <= (others=>'0');
end architecture blackbox;

-- ---- rp761q4591encode_param ---------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
entity rp761q4591encode_param is
    port (
        state_l         : in  std_logic_vector(4 downto 0);
        state_s         : in  std_logic_vector(4 downto 0);
        state_max       : out std_logic_vector(4 downto 0);
        param_r_max     : out std_logic_vector(9 downto 0);
        param_m0        : out std_logic_vector(13 downto 0);
        param_1st_round : out std_logic;
        param_outs1     : out std_logic_vector(2 downto 0);
        param_outsl     : out std_logic_vector(2 downto 0)
    );
end entity rp761q4591encode_param;
architecture blackbox of rp761q4591encode_param is
begin
    state_max <= (others=>'0'); param_r_max <= (others=>'0');
    param_m0 <= (others=>'0'); param_1st_round <= '0';
    param_outs1 <= (others=>'0'); param_outsl <= (others=>'0');
end architecture blackbox;

-- ---- rp761q1531encode_param (same ports as q4591) ------------------------
library ieee;
use ieee.std_logic_1164.all;
entity rp761q1531encode_param is
    port (
        state_l         : in  std_logic_vector(4 downto 0);
        state_s         : in  std_logic_vector(4 downto 0);
        state_max       : out std_logic_vector(4 downto 0);
        param_r_max     : out std_logic_vector(9 downto 0);
        param_m0        : out std_logic_vector(13 downto 0);
        param_1st_round : out std_logic;
        param_outs1     : out std_logic_vector(2 downto 0);
        param_outsl     : out std_logic_vector(2 downto 0)
    );
end entity rp761q1531encode_param;
architecture blackbox of rp761q1531encode_param is
begin
    state_max <= (others=>'0'); param_r_max <= (others=>'0');
    param_m0 <= (others=>'0'); param_1st_round <= '0';
    param_outs1 <= (others=>'0'); param_outsl <= (others=>'0');
end architecture blackbox;
