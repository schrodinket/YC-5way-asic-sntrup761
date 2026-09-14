library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity modmul4591S is
    port (
        Clk    : in  std_logic;
        Reset  : in  std_logic;
        Input  : in  std_logic_vector(27 downto 0);
        Output : out std_logic_vector(12 downto 0)
    );
end entity modmul4591S;

architecture rtl of modmul4591S is
    signal regZ    : signed(27 downto 0);
    signal intP0   : std_logic_vector(11 downto 0);
    signal intP1   : std_logic_vector(11 downto 0);
    signal intN0   : std_logic_vector(11 downto 0);
    signal intN1   : std_logic_vector(11 downto 0);
    signal intP    : unsigned(12 downto 0);
    signal intN    : unsigned(12 downto 0);
    signal regD    : signed(13 downto 0);
    signal intMODQ : signed(14 downto 0);
    signal intAB   : signed(14 downto 0);
begin

    -- regZ <= Input
    process(Clk)
    begin
        if rising_edge(Clk) then
            if Reset = '1' then
                regZ <= (others => '0');
            else
                regZ <= signed(Input);
            end if;
        end if;
    end process;

    -- Lookup table
    lut : entity work.mod4591Svec28
        port map (
            z_in => std_logic_vector(regZ),
            p0   => intP0,
            p1   => intP1,
            n0   => intN0,
            n1   => intN1
        );

    intP <= resize(unsigned(intP0), 13) + resize(unsigned(intP1), 13);
    intN <= resize(unsigned(intN0), 13) + resize(unsigned(intN1), 13);

    -- regD <= {1'b0, intP} - {1'b0, intN}
    process(Clk)
    begin
        if rising_edge(Clk) then
            regD <= signed('0' & intP) - signed('0' & intN);
        end if;
    end process;

    -- intMODQ lookup (case on regD[13:10])
    process(regD)
    begin
        case std_logic_vector(regD(13 downto 10)) is
            when "0111" => intMODQ <= to_signed(-9182, 15);   -- 4'h7
            when "0010" | "0011" | "0100" | "0101" | "0110" =>
                           intMODQ <= to_signed(-4591, 15);   -- 4'h2..6
            when "1110" | "1111" | "0000" | "0001" =>
                           intMODQ <= to_signed(0, 15);       -- 4'he,f,0,1
            when "1001" | "1010" | "1011" | "1100" | "1101" =>
                           intMODQ <= to_signed(4591, 15);    -- 4'h9..d
            when "1000" => intMODQ <= to_signed(9182, 15);    -- 4'h8
            when others => intMODQ <= (others => '0');
        end case;
    end process;

    -- intAB = {regD[13], regD} + intMODQ
    intAB <= resize(regD, 15) + intMODQ;

    -- Output with range correction
    process(Clk)
        variable v : signed(12 downto 0);
    begin
        if rising_edge(Clk) then
            if Reset = '1' then
                Output <= (others => '0');
            else
                v := intAB(12 downto 0);
                if v > 2295 then
                    Output <= std_logic_vector(v - 4591);
                elsif v < -2295 then
                    Output <= std_logic_vector(v + 4591);
                else
                    Output <= std_logic_vector(v);
                end if;
            end if;
        end if;
    end process;

end architecture rtl;
