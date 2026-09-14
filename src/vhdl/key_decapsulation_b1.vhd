library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.constants.all;
use work.data_type.all;

-- The core decapsulation
entity key_decapsulation_b1 is
	port(
		clock                : in  std_logic;
		reset                : in  std_logic;
		start                : in  std_logic;
		done                 : out std_logic;
		output               : out std_logic_vector(1 downto 0);
		output_valid         : out std_logic;
		key_ready            : in  std_logic;
		bram_ginv_address_a  : out std_logic_vector(p_num_bits - 1 downto 0);
		bram_ginv_data_out_a : in  std_logic_vector(1 downto 0);
		bram_ginv_address_b  : out std_logic_vector(p_num_bits - 1 downto 0);
		bram_ginv_data_out_b : in  std_logic_vector(1 downto 0);
		f_address_a          : out std_logic_vector(p_num_bits - 1 downto 0);
		f_data_out_a         : in  std_logic_vector(1 downto 0);
		f_address_b          : out std_logic_vector(p_num_bits - 1 downto 0);
		f_data_out_b         : in  std_logic_vector(1 downto 0);
		c_address_a          : out std_logic_vector(p_num_bits - 1 downto 0);
		c_data_out_a         : in  std_logic_vector(q_num_bits - 1 downto 0);
		c_address_b          : out std_logic_vector(p_num_bits - 1 downto 0);
		c_data_out_b         : in  std_logic_vector(q_num_bits - 1 downto 0);
		to_rq_mult           : out rq_multiplication_in_type;
		from_rq_mult         : in  rq_multiplication_out_type;
		to_freeze_round      : out mod3_freeze_round_in_type;
		from_freeze_round    : in  mod3_freeze_round_out_type
	);
end entity key_decapsulation_b1;

architecture RTL of key_decapsulation_b1 is

	type state_type is (idle, mult_Rq, mult_freeze, mult_freeze_done, mult_R3, calc_weight, output_masked_weight, done_state);
	signal state_decap : state_type;

	signal rq_mult3_enable       : std_logic;
	signal rq_mult3_input        : std_logic_vector(q_num_bits - 1 downto 0);
	signal rq_mult3_output       : std_logic_vector(q_num_bits - 1 downto 0);
	signal rq_mult3_output_valid : std_logic;

	signal r3_mult_start        : std_logic;
	signal r3_mult_ready        : std_logic;
	signal r3_mult_output_valid : std_logic;
	signal r3_mult_output       : std_logic_vector(1 downto 0);
	signal r3_mult_done         : std_logic;

	signal rq_mult_start        : std_logic;
	signal rq_mult_ready        : std_logic;
	signal rq_mult_output_valid : std_logic;
	signal rq_mult_output       : std_logic_vector(q_num_bits - 1 downto 0);
	signal rq_mult_output_ack   : std_logic;
	signal rq_mult_done         : std_logic;
	signal rq_mult_load_small   : std_logic;

	signal calc_weight_start             : std_logic;
	signal calc_weight_input             : std_logic_vector(1 downto 0);
	signal calc_weight_input_valid       : std_logic;
	signal calc_weight_mask_output       : std_logic_vector(q_num_bits - 1 downto 0);
	signal calc_weight_mask_output_valid : std_logic;

	signal mod3_freeze_input        : signed(q_num_bits - 1 downto 0);
	signal mod3_freeze_enable       : std_logic;
	signal mod3_freeze_output       : signed(1 downto 0);
	signal mod3_freeze_output_valid : std_logic;

	signal bram_e_address_a  : std_logic_vector(p_num_bits - 1 downto 0);
	signal bram_e_write_a    : std_logic;
	signal bram_e_data_in_a  : std_logic_vector(1 downto 0);
	signal bram_e_data_out_a : std_logic_vector(1 downto 0);
	signal bram_e_address_b  : std_logic_vector(p_num_bits - 1 downto 0);
	signal bram_e_write_b    : std_logic;
	signal bram_e_data_in_b  : std_logic_vector(1 downto 0);
	signal bram_e_data_out_b : std_logic_vector(1 downto 0);

	signal bram_e_address_a_r3 : std_logic_vector(p_num_bits - 1 downto 0);
	signal bram_e_address_b_r3 : std_logic_vector(p_num_bits - 1 downto 0);

	signal bram_e_address_a_fsm : std_logic_vector(p_num_bits - 1 downto 0);

	signal counter : integer range 0 to p;
	signal drain_cnt : integer range 0 to 15 := 0;

	signal ev : std_logic_vector(1 downto 0);
	signal ev_raw : std_logic_vector(1 downto 0);

	signal key_ready_pipe : std_logic;

	-- B1 multiplier interface signals
	signal e_wide       : std_logic_vector(2*p - 1 downto 0);  -- e in Peng encoding
	signal ginv_wide    : std_logic_vector(2*p - 1 downto 0);  -- g^-1 in Peng encoding
	signal b1_start     : std_logic;
	signal b1_done      : std_logic;
	signal b1_ov        : std_logic;
	signal b1_output    : std_logic_vector(1 downto 0);
	signal r3_started   : std_logic;  -- single-cycle start pulse
	signal b1_result_wide : std_logic_vector(2*p - 1 downto 0);  -- B1 result, 2*p bits
	signal b1_result_ready: std_logic;
	signal cwp_start      : std_logic;  -- parallel weight start
	signal cwp_mask       : std_logic_vector(q_num_bits - 1 downto 0);
	signal cwp_valid      : std_logic;
	-- cycle counters (observation only, does not affect FSM logic)
	signal cyc_rq, cyc_freeze, cyc_fdone, cyc_r3, cyc_weight, cyc_output : integer := 0;

	component r3_b1_bridge is
		generic (P : integer := 761; W : integer := 1522);
		port (
			clk          : in  std_logic;
			rst          : in  std_logic;
			start        : in  std_logic;
			e_peng       : in  std_logic_vector(2*p - 1 downto 0);
			ginv_peng    : in  std_logic_vector(2*p - 1 downto 0);
			done         : out std_logic;
			output_valid : out std_logic;
			r3_output    : out std_logic_vector(1 downto 0);
			result_wide  : out std_logic_vector(2*p - 1 downto 0);
			result_ready : out std_logic
		);
	end component;

begin

	-- observation-only cycle counter per FSM state
	cycle_count_proc : process(clock) is
	begin
		if rising_edge(clock) then
			case state_decap is
				when mult_Rq              => cyc_rq     <= cyc_rq + 1;
				when mult_freeze          => cyc_freeze <= cyc_freeze + 1;
				when mult_freeze_done     => cyc_fdone  <= cyc_fdone + 1;
				when mult_R3              => cyc_r3     <= cyc_r3 + 1;
				when calc_weight          => cyc_weight <= cyc_weight + 1;
				when output_masked_weight => cyc_output <= cyc_output + 1;
				when done_state =>
					report "DECAP_CYCLES rq=" & integer'image(cyc_rq) &
						" freeze=" & integer'image(cyc_freeze) &
						" fdone=" & integer'image(cyc_fdone) &
						" r3=" & integer'image(cyc_r3) &
						" weight=" & integer'image(cyc_weight) &
						" output=" & integer'image(cyc_output) &
						" TOTAL=" & integer'image(cyc_rq+cyc_freeze+cyc_fdone+cyc_r3+cyc_weight+cyc_output);
				when others => null;
			end case;
		end if;
	end process cycle_count_proc;

	fsm_process : process(clock, reset) is
	begin
		if reset = '1' then
			state_decap        <= idle;
			done               <= '0';
			calc_weight_start  <= '0';
			r3_mult_start      <= '0';
			--rq_mult3_enable   <= '0';
			rq_mult_start      <= '0';
			output_valid       <= '0';
			rq_mult_load_small <= '0';
			cwp_start          <= '0';
		elsif rising_edge(clock) then
			case state_decap is
				when idle =>
					if start = '1' then
						state_decap   <= mult_Rq;
						rq_mult_start <= '1';
						done          <= '0';
					end if;

					counter <= 0;

					if key_ready = '1' and key_ready_pipe = '0' then
						rq_mult_load_small <= '1';
					else
						rq_mult_load_small <= '0';
					end if;

				when mult_Rq =>
					rq_mult_load_small <= '0';
					if rq_mult_output_valid = '1' then
						state_decap <= mult_freeze;
						rq_mult_load_small <= '1';
					end if;

					rq_mult_start <= '0';
				when mult_freeze =>
					if mod3_freeze_output_valid = '1' then
						counter <= counter + 1;
					end if;
					if rq_mult_done = '1' then
						state_decap <= mult_freeze_done;
					end if;
					
					rq_mult_load_small <= '0';
				when mult_freeze_done =>
					rq_mult_load_small <= '0';
					if mod3_freeze_output_valid = '1' then
						counter <= counter + 1;
					end if;
					if mod3_freeze_output_valid = '0' and mod3_freeze_enable = '0' then
					    if drain_cnt >= 4 then
    						state_decap       <= mult_R3;
    						r3_mult_start     <= '1';
    						calc_weight_start <= '1';
    						counter           <= 0;
					        drain_cnt <= 0;
					    else
					        drain_cnt <= drain_cnt + 1;
					    end if;
					end if;
				when mult_R3 =>
					-- start parallel weight once B1 result is ready
					r3_mult_start     <= '0';
					if b1_result_ready = '1' then
						cwp_start   <= '1';   -- start parallel popcount
						state_decap <= calc_weight;
						counter     <= 0;
					end if;
				when calc_weight =>
					-- move to output once weight is done
					cwp_start <= '0';
					if cwp_valid = '1' then
						state_decap <= output_masked_weight;
						output_valid <= '1';  -- assert one cycle early so position 0 is not missed
						counter     <= 0;
					end if;
				when output_masked_weight =>
					counter      <= counter + 1;
					output_valid <= '1';
					if counter = p - 1 then -- p samples
						state_decap  <= done_state;
						output_valid <= '0';
					end if;
				when done_state =>
					done <= '1';

					state_decap <= idle;
			end case;
		end if;
	end process fsm_process;

	key_ready_pipe <= key_ready when rising_edge(clock);

	-- capture e and g^-1 during mult_freeze into wide registers
	-- e written from mod3_freeze_output
	-- g^-1 read has one cycle BRAM latency
	capture_proc : process(clock, reset) is
	    variable cap_idx   : integer range 0 to p := 0;
	    variable gaddr_d   : integer range 0 to p := 0;
	    variable capturing : std_logic := '0';
	begin
	    if reset = '1' then
	        cap_idx   := 0;
	        gaddr_d   := 0;
	        capturing := '0';
	        e_wide    <= (others => '0');
	        ginv_wide <= (others => '0');
	    elsif rising_edge(clock) then
	        if state_decap = mult_freeze and capturing = '0' and cap_idx = 0 then
	            capturing := '1';
	        end if;
	        if capturing = '1' then
	            if mod3_freeze_output_valid = '1' then
	                e_wide(2*cap_idx + 1 downto 2*cap_idx) <= std_logic_vector(mod3_freeze_output);
	                ginv_wide(2*gaddr_d + 1 downto 2*gaddr_d) <= bram_ginv_data_out_a;
	                gaddr_d := cap_idx;
	                if cap_idx = p - 1 then
	                    cap_idx := cap_idx + 1;
	                else
	                    cap_idx := cap_idx + 1;
	                end if;
	            elsif cap_idx = p then
	                -- write last g^-1 coefficient
	                ginv_wide(2*gaddr_d + 1 downto 2*gaddr_d) <= bram_ginv_data_out_a;
	                capturing := '0';
	                cap_idx   := 0;  -- reset counters for next decapsulation
	                gaddr_d   := 0;
	            end if;
	        end if;
	    end if;
	end process capture_proc;
	
	rq_mult_output_ack <= '1';

	rq_mult3_input  <= rq_mult_output;
	rq_mult3_enable <= rq_mult_output_valid when state_decap /= mult_R3 and state_decap /= calc_weight else '0';

	mod3_freeze_input  <= signed(rq_mult3_output) when state_decap /= mult_R3 and state_decap /= calc_weight else signed(from_rq_mult.output);
	mod3_freeze_enable <= rq_mult3_output_valid when state_decap /= mult_R3 and state_decap /= calc_weight else from_rq_mult.output_valid;

	bram_e_address_a_fsm <= std_logic_vector(to_unsigned(counter, p_num_bits));
	bram_e_write_a       <= mod3_freeze_output_valid when state_decap /= mult_R3 and state_decap /= calc_weight else r3_mult_output_valid;
	bram_e_data_in_a     <= std_logic_vector(mod3_freeze_output) when state_decap /= mult_R3 and state_decap /= calc_weight else r3_mult_output;

	bram_e_address_a <= bram_e_address_a_fsm when state_decap = mult_freeze or --
	                    state_decap = mult_freeze_done or --
	                    state_decap = output_masked_weight or r3_mult_output_valid = '1' else bram_e_address_a_r3;

	bram_e_address_b <= bram_e_address_b_r3;

	bram_e_write_b   <= '0';
	bram_e_data_in_b <= (others => '0');

	calc_weight_input_valid <= r3_mult_output_valid;
	calc_weight_input       <= r3_mult_output;

	-- output served from B1 register
	-- guard against overflow at counter = p
	ev_raw     <= b1_result_wide(2*counter + 1 downto 2*counter) when (state_decap = output_masked_weight and counter < p) else (others => '0');
	ev     <= "11" when ev_raw = "10" else ev_raw;  -- map B1 encoding (-1=10) to Peng encoding (-1=11)
	output <= ((ev xor "01") and not cwp_mask(1 downto 0)) xor "01" when counter <= 2 * t --
	          else ev and not cwp_mask(1 downto 0);

	rq_mult3_inst : entity work.rq_mult3
		port map(
			clock        => clock,
			reset        => reset,
			enable       => rq_mult3_enable,
			input        => rq_mult3_input,
			output       => rq_mult3_output,
			output_valid => rq_mult3_output_valid
		);

	r3_mult_ready       <= from_rq_mult.ready;
	r3_mult_done        <= b1_done;  -- bridge done
	bram_e_address_a_r3 <= from_rq_mult.bram_f_address_a;
	bram_e_address_b_r3 <= from_rq_mult.bram_f_address_b;
	-- g^-1 address: counter during capture, engine otherwise
	bram_ginv_address_a <= std_logic_vector(to_unsigned(counter, p_num_bits))
	                       when state_decap = mult_freeze or state_decap = mult_freeze_done
	                       else from_rq_mult.bram_g_address_a;
	bram_ginv_address_b <= from_rq_mult.bram_g_address_b;

	r3_mult_output_valid <= b1_ov when state_decap = mult_R3 or state_decap = calc_weight else '0';
	r3_mult_output       <= b1_output;

	rq_mult_ready <= from_rq_mult.ready;
	rq_mult_done  <= from_rq_mult.done;
	c_address_a   <= from_rq_mult.bram_f_address_a;
	c_address_b   <= from_rq_mult.bram_f_address_b;
	f_address_a   <= from_rq_mult.bram_g_address_a;
	f_address_b   <= from_rq_mult.bram_g_address_b;

	rq_mult_output_valid <= from_rq_mult.output_valid;
	rq_mult_output       <= from_rq_mult.output;

	to_rq_mult.start        <= rq_mult_start when state_decap = idle or state_decap = mult_Rq else '0';  -- do not start rq_mult during mult_R3; B1 handles it
	to_rq_mult.output_ack   <= '1';
	to_rq_mult.load_small_g <= rq_mult_load_small when state_decap = idle or state_decap = mult_Rq else '0';  -- do not load_small during mult_freeze/R3; B1 handles R/3

	to_rq_mult.bram_f_data_out_a <= c_data_out_a when state_decap = idle or state_decap = mult_Rq else std_logic_vector(resize(signed(bram_e_data_out_a), q_num_bits));
	to_rq_mult.bram_f_data_out_b <= c_data_out_b when state_decap = idle or state_decap = mult_Rq else std_logic_vector(resize(signed(bram_e_data_out_b), q_num_bits));
	to_rq_mult.bram_g_data_out_a <= f_data_out_a when state_decap = idle or state_decap = mult_Rq else bram_ginv_data_out_a;
	to_rq_mult.bram_g_data_out_b <= f_data_out_b when state_decap = idle or state_decap = mult_Rq else bram_ginv_data_out_b;

	-- B1 R/3 bridge instance
	-- b1_start pulses for one cycle on entering mult_R3
	b1_start <= r3_mult_start;

	r3_b1_bridge_inst : r3_b1_bridge
		generic map (P => p, W => 2*p)
		port map (
			clk          => clock,
			rst          => reset,
			start        => b1_start,
			e_peng       => e_wide,
			ginv_peng    => ginv_wide,
			done         => b1_done,
			output_valid => b1_ov,
			r3_output    => b1_output,
			result_wide  => b1_result_wide,
			result_ready => b1_result_ready
		);

	-- parallel weight (popcount)
	calc_weight_par_inst : entity work.calc_weight_par
		port map(
			clock             => clock,
			reset             => reset,
			start             => cwp_start,
			data_wide         => b1_result_wide,
			mask_output       => cwp_mask,
			mask_output_valid => cwp_valid
		);

	to_freeze_round.input  <= mod3_freeze_input;
	to_freeze_round.enable <= mod3_freeze_enable;

	mod3_freeze_output       <= from_freeze_round.output;
	mod3_freeze_output_valid <= from_freeze_round.output_valid;

	block_ram_inst : entity work.block_ram
		generic map(
			ADDRESS_WIDTH => p_num_bits,
			DATA_WIDTH    => 2
		)
		port map(
			clock      => clock,
			address_a  => bram_e_address_a,
			write_a    => bram_e_write_a,
			data_in_a  => bram_e_data_in_a,
			data_out_a => bram_e_data_out_a,
			address_b  => bram_e_address_b,
			write_b    => bram_e_write_b,
			data_in_b  => bram_e_data_in_b,
			data_out_b => bram_e_data_out_b
		);

end architecture RTL;
