library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- Generic dual port, single clock block RAM
entity block_ram is
	Generic(
		ADDRESS_WIDTH : integer := 8;
		DATA_WIDTH    : integer := 8;
		DUAL_PORT     : boolean := TRUE
	);
	Port(clock      : in  STD_LOGIC;
	     address_a  : in  STD_LOGIC_VECTOR(ADDRESS_WIDTH - 1 downto 0);
	     write_a    : in  STD_LOGIC;
	     data_in_a  : in  STD_LOGIC_VECTOR(DATA_WIDTH - 1 downto 0);
	     data_out_a : out STD_LOGIC_VECTOR(DATA_WIDTH - 1 downto 0);
	     address_b  : in  STD_LOGIC_VECTOR(ADDRESS_WIDTH - 1 downto 0);
	     write_b    : in  STD_LOGIC;
	     data_in_b  : in  STD_LOGIC_VECTOR(DATA_WIDTH - 1 downto 0);
	     data_out_b : out STD_LOGIC_VECTOR(DATA_WIDTH - 1 downto 0)
	    );
end entity block_ram;
architecture rtl of block_ram is
    type memory is array (0 to (2**ADDRESS_WIDTH) - 1) of STD_LOGIC_VECTOR(DATA_WIDTH - 1 downto 0);
    signal RAM : memory;
begin

    -- Single process handles both ports (avoids multiple-driver on RAM)
    ram_proc : process(clock) is
    begin
        if rising_edge(clock) then
            -- Port A
            if write_a = '1' then
                RAM(to_integer(unsigned(address_a))) <= data_in_a;
            end if;
            data_out_a <= RAM(to_integer(unsigned(address_a)));

            -- Port B (only if DUAL_PORT)
            if DUAL_PORT then
                if write_b = '1' then
                    RAM(to_integer(unsigned(address_b))) <= data_in_b;
                end if;
                data_out_b <= RAM(to_integer(unsigned(address_b)));
            end if;
        end if;
    end process ram_proc;

end architecture rtl;
