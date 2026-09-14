library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.constants.all;
use work.data_type.all;

-- Parallel weight check. Counts the nonzero coefficients (popcount) of the
-- 1522-bit (2*p) R/3 result in one pass.
-- Peng encoding: each coefficient is 2-bit, bit0 marks nonzero (0=00, +1=01, -1=11).
-- weight = sum(bit0). mask = (weight == 2*t) ? 0 : all-ones.
-- mask_output_valid asserts a few cycles after start.
entity calc_weight_par is
	port(
		clock             : in  std_logic;
		reset             : in  std_logic;
		start             : in  std_logic;
		data_wide         : in  std_logic_vector(2*p - 1 downto 0);
		mask_output       : out std_logic_vector(q_num_bits - 1 downto 0);
		mask_output_valid : out std_logic
	);
end entity calc_weight_par;

architecture RTL of calc_weight_par is
	type state_type is (idle, sum_state, mask_state, done_state);
	signal st : state_type;
	signal weight : integer range 0 to p;
	signal signed_mask : signed(q_num_bits - 1 downto 0);
begin
	process(clock, reset) is
		variable acc : integer range 0 to p;
	begin
		if reset = '1' then
			st <= idle;
			mask_output_valid <= '0';
		elsif rising_edge(clock) then
			case st is
				when idle =>
					mask_output_valid <= '0';
					if start = '1' then
						st <= sum_state;
					end if;
				when sum_state =>
					-- popcount: sum of bit0 over all coefficients
					acc := 0;
					for i in 0 to p - 1 loop
						if (data_wide(2*i) = '1' or data_wide(2*i+1) = '1') then
							acc := acc + 1;
						end if;
					end loop;
					weight <= acc;
					st <= mask_state;
				when mask_state =>
					signed_mask <= non_zero_mask(to_signed(weight - 2 * t, q_num_bits))(q_num_bits - 1 downto 0);
					st <= done_state;
				when done_state =>
					mask_output       <= std_logic_vector(signed_mask);
					mask_output_valid <= '1';
					st <= idle;
			end case;
		end if;
	end process;
end architecture RTL;
