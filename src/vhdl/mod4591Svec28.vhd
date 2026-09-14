library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity mod4591Svec28 is
    port (
        z_in : in  std_logic_vector(27 downto 0);
        p0   : out std_logic_vector(11 downto 0);
        p1   : out std_logic_vector(11 downto 0);
        n0   : out std_logic_vector(11 downto 0);
        n1   : out std_logic_vector(11 downto 0)
    );
end entity mod4591Svec28;

architecture rtl of mod4591Svec28 is
    signal sel_n0 : std_logic_vector(4 downto 0);
    signal sel_n1 : std_logic_vector(5 downto 0);
    signal sel_p1 : std_logic_vector(5 downto 0);
begin

    sel_n0 <= z_in(24) & z_in(23) & z_in(22) & z_in(16) & z_in(12);
    sel_n1 <= z_in(25) & z_in(23) & z_in(21) & z_in(20) & z_in(19) & z_in(13);
    sel_p1 <= z_in(27) & z_in(26) & z_in(18) & z_in(17) & z_in(15) & z_in(14);

    -- n0: 32 entries
    process(sel_n0)
    begin
        case sel_n0 is
            when "00000" => n0 <= std_logic_vector(to_unsigned(0, 12));
            when "00001" => n0 <= std_logic_vector(to_unsigned(495, 12));
            when "00010" => n0 <= std_logic_vector(to_unsigned(3329, 12));
            when "00011" => n0 <= std_logic_vector(to_unsigned(3824, 12));
            when "00100" => n0 <= std_logic_vector(to_unsigned(1870, 12));
            when "00101" => n0 <= std_logic_vector(to_unsigned(2365, 12));
            when "00110" => n0 <= std_logic_vector(to_unsigned(608, 12));
            when "00111" => n0 <= std_logic_vector(to_unsigned(1103, 12));
            when "01000" => n0 <= std_logic_vector(to_unsigned(28, 12));
            when "01001" => n0 <= std_logic_vector(to_unsigned(523, 12));
            when "01010" => n0 <= std_logic_vector(to_unsigned(3357, 12));
            when "01011" => n0 <= std_logic_vector(to_unsigned(3852, 12));
            when "01100" => n0 <= std_logic_vector(to_unsigned(1898, 12));
            when "01101" => n0 <= std_logic_vector(to_unsigned(2393, 12));
            when "01110" => n0 <= std_logic_vector(to_unsigned(636, 12));
            when "01111" => n0 <= std_logic_vector(to_unsigned(1131, 12));
            when "10000" => n0 <= std_logic_vector(to_unsigned(2889, 12));
            when "10001" => n0 <= std_logic_vector(to_unsigned(3384, 12));
            when "10010" => n0 <= std_logic_vector(to_unsigned(1627, 12));
            when "10011" => n0 <= std_logic_vector(to_unsigned(2122, 12));
            when "10100" => n0 <= std_logic_vector(to_unsigned(168, 12));
            when "10101" => n0 <= std_logic_vector(to_unsigned(663, 12));
            when "10110" => n0 <= std_logic_vector(to_unsigned(3497, 12));
            when "10111" => n0 <= std_logic_vector(to_unsigned(3992, 12));
            when "11000" => n0 <= std_logic_vector(to_unsigned(2917, 12));
            when "11001" => n0 <= std_logic_vector(to_unsigned(3412, 12));
            when "11010" => n0 <= std_logic_vector(to_unsigned(1655, 12));
            when "11011" => n0 <= std_logic_vector(to_unsigned(2150, 12));
            when "11100" => n0 <= std_logic_vector(to_unsigned(196, 12));
            when "11101" => n0 <= std_logic_vector(to_unsigned(691, 12));
            when "11110" => n0 <= std_logic_vector(to_unsigned(3525, 12));
            when "11111" => n0 <= std_logic_vector(to_unsigned(4020, 12));
            when others => n0 <= (others => '0');
        end case;
    end process;

    -- n1: 64 entries
    process(sel_n1)
    begin
        case sel_n1 is
            when "000000" => n1 <= std_logic_vector(to_unsigned(0, 12));
            when "000001" => n1 <= std_logic_vector(to_unsigned(990, 12));
            when "000010" => n1 <= std_logic_vector(to_unsigned(3677, 12));
            when "000011" => n1 <= std_logic_vector(to_unsigned(76, 12));
            when "000100" => n1 <= std_logic_vector(to_unsigned(2763, 12));
            when "000101" => n1 <= std_logic_vector(to_unsigned(3753, 12));
            when "000110" => n1 <= std_logic_vector(to_unsigned(1849, 12));
            when "000111" => n1 <= std_logic_vector(to_unsigned(2839, 12));
            when "001000" => n1 <= std_logic_vector(to_unsigned(935, 12));
            when "001001" => n1 <= std_logic_vector(to_unsigned(1925, 12));
            when "001010" => n1 <= std_logic_vector(to_unsigned(21, 12));
            when "001011" => n1 <= std_logic_vector(to_unsigned(1011, 12));
            when "001100" => n1 <= std_logic_vector(to_unsigned(3698, 12));
            when "001101" => n1 <= std_logic_vector(to_unsigned(97, 12));
            when "001110" => n1 <= std_logic_vector(to_unsigned(2784, 12));
            when "001111" => n1 <= std_logic_vector(to_unsigned(3774, 12));
            when "010000" => n1 <= std_logic_vector(to_unsigned(3712, 12));
            when "010001" => n1 <= std_logic_vector(to_unsigned(111, 12));
            when "010010" => n1 <= std_logic_vector(to_unsigned(2798, 12));
            when "010011" => n1 <= std_logic_vector(to_unsigned(3788, 12));
            when "010100" => n1 <= std_logic_vector(to_unsigned(1884, 12));
            when "010101" => n1 <= std_logic_vector(to_unsigned(2874, 12));
            when "010110" => n1 <= std_logic_vector(to_unsigned(970, 12));
            when "010111" => n1 <= std_logic_vector(to_unsigned(1960, 12));
            when "011000" => n1 <= std_logic_vector(to_unsigned(56, 12));
            when "011001" => n1 <= std_logic_vector(to_unsigned(1046, 12));
            when "011010" => n1 <= std_logic_vector(to_unsigned(3733, 12));
            when "011011" => n1 <= std_logic_vector(to_unsigned(132, 12));
            when "011100" => n1 <= std_logic_vector(to_unsigned(2819, 12));
            when "011101" => n1 <= std_logic_vector(to_unsigned(3809, 12));
            when "011110" => n1 <= std_logic_vector(to_unsigned(1905, 12));
            when "011111" => n1 <= std_logic_vector(to_unsigned(2895, 12));
            when "100000" => n1 <= std_logic_vector(to_unsigned(1187, 12));
            when "100001" => n1 <= std_logic_vector(to_unsigned(2177, 12));
            when "100010" => n1 <= std_logic_vector(to_unsigned(273, 12));
            when "100011" => n1 <= std_logic_vector(to_unsigned(1263, 12));
            when "100100" => n1 <= std_logic_vector(to_unsigned(3950, 12));
            when "100101" => n1 <= std_logic_vector(to_unsigned(349, 12));
            when "100110" => n1 <= std_logic_vector(to_unsigned(3036, 12));
            when "100111" => n1 <= std_logic_vector(to_unsigned(4026, 12));
            when "101000" => n1 <= std_logic_vector(to_unsigned(2122, 12));
            when "101001" => n1 <= std_logic_vector(to_unsigned(3112, 12));
            when "101010" => n1 <= std_logic_vector(to_unsigned(1208, 12));
            when "101011" => n1 <= std_logic_vector(to_unsigned(2198, 12));
            when "101100" => n1 <= std_logic_vector(to_unsigned(294, 12));
            when "101101" => n1 <= std_logic_vector(to_unsigned(1284, 12));
            when "101110" => n1 <= std_logic_vector(to_unsigned(3971, 12));
            when "101111" => n1 <= std_logic_vector(to_unsigned(370, 12));
            when "110000" => n1 <= std_logic_vector(to_unsigned(308, 12));
            when "110001" => n1 <= std_logic_vector(to_unsigned(1298, 12));
            when "110010" => n1 <= std_logic_vector(to_unsigned(3985, 12));
            when "110011" => n1 <= std_logic_vector(to_unsigned(384, 12));
            when "110100" => n1 <= std_logic_vector(to_unsigned(3071, 12));
            when "110101" => n1 <= std_logic_vector(to_unsigned(4061, 12));
            when "110110" => n1 <= std_logic_vector(to_unsigned(2157, 12));
            when "110111" => n1 <= std_logic_vector(to_unsigned(3147, 12));
            when "111000" => n1 <= std_logic_vector(to_unsigned(1243, 12));
            when "111001" => n1 <= std_logic_vector(to_unsigned(2233, 12));
            when "111010" => n1 <= std_logic_vector(to_unsigned(329, 12));
            when "111011" => n1 <= std_logic_vector(to_unsigned(1319, 12));
            when "111100" => n1 <= std_logic_vector(to_unsigned(4006, 12));
            when "111101" => n1 <= std_logic_vector(to_unsigned(405, 12));
            when "111110" => n1 <= std_logic_vector(to_unsigned(3092, 12));
            when "111111" => n1 <= std_logic_vector(to_unsigned(4082, 12));
            when others => n1 <= (others => '0');
        end case;
    end process;

    -- p1: 64 entries
    process(sel_p1)
    begin
        case sel_p1 is
            when "000000" => p1 <= std_logic_vector(to_unsigned(0, 12));
            when "000001" => p1 <= std_logic_vector(to_unsigned(2611, 12));
            when "000010" => p1 <= std_logic_vector(to_unsigned(631, 12));
            when "000011" => p1 <= std_logic_vector(to_unsigned(3242, 12));
            when "000100" => p1 <= std_logic_vector(to_unsigned(2524, 12));
            when "000101" => p1 <= std_logic_vector(to_unsigned(544, 12));
            when "000110" => p1 <= std_logic_vector(to_unsigned(3155, 12));
            when "000111" => p1 <= std_logic_vector(to_unsigned(1175, 12));
            when "001000" => p1 <= std_logic_vector(to_unsigned(457, 12));
            when "001001" => p1 <= std_logic_vector(to_unsigned(3068, 12));
            when "001010" => p1 <= std_logic_vector(to_unsigned(1088, 12));
            when "001011" => p1 <= std_logic_vector(to_unsigned(3699, 12));
            when "001100" => p1 <= std_logic_vector(to_unsigned(2981, 12));
            when "001101" => p1 <= std_logic_vector(to_unsigned(1001, 12));
            when "001110" => p1 <= std_logic_vector(to_unsigned(3612, 12));
            when "001111" => p1 <= std_logic_vector(to_unsigned(1632, 12));
            when "010000" => p1 <= std_logic_vector(to_unsigned(2217, 12));
            when "010001" => p1 <= std_logic_vector(to_unsigned(237, 12));
            when "010010" => p1 <= std_logic_vector(to_unsigned(2848, 12));
            when "010011" => p1 <= std_logic_vector(to_unsigned(868, 12));
            when "010100" => p1 <= std_logic_vector(to_unsigned(150, 12));
            when "010101" => p1 <= std_logic_vector(to_unsigned(2761, 12));
            when "010110" => p1 <= std_logic_vector(to_unsigned(781, 12));
            when "010111" => p1 <= std_logic_vector(to_unsigned(3392, 12));
            when "011000" => p1 <= std_logic_vector(to_unsigned(2674, 12));
            when "011001" => p1 <= std_logic_vector(to_unsigned(694, 12));
            when "011010" => p1 <= std_logic_vector(to_unsigned(3305, 12));
            when "011011" => p1 <= std_logic_vector(to_unsigned(1325, 12));
            when "011100" => p1 <= std_logic_vector(to_unsigned(607, 12));
            when "011101" => p1 <= std_logic_vector(to_unsigned(3218, 12));
            when "011110" => p1 <= std_logic_vector(to_unsigned(1238, 12));
            when "011111" => p1 <= std_logic_vector(to_unsigned(3849, 12));
            when "100000" => p1 <= std_logic_vector(to_unsigned(157, 12));
            when "100001" => p1 <= std_logic_vector(to_unsigned(2768, 12));
            when "100010" => p1 <= std_logic_vector(to_unsigned(788, 12));
            when "100011" => p1 <= std_logic_vector(to_unsigned(3399, 12));
            when "100100" => p1 <= std_logic_vector(to_unsigned(2681, 12));
            when "100101" => p1 <= std_logic_vector(to_unsigned(701, 12));
            when "100110" => p1 <= std_logic_vector(to_unsigned(3312, 12));
            when "100111" => p1 <= std_logic_vector(to_unsigned(1332, 12));
            when "101000" => p1 <= std_logic_vector(to_unsigned(614, 12));
            when "101001" => p1 <= std_logic_vector(to_unsigned(3225, 12));
            when "101010" => p1 <= std_logic_vector(to_unsigned(1245, 12));
            when "101011" => p1 <= std_logic_vector(to_unsigned(3856, 12));
            when "101100" => p1 <= std_logic_vector(to_unsigned(3138, 12));
            when "101101" => p1 <= std_logic_vector(to_unsigned(1158, 12));
            when "101110" => p1 <= std_logic_vector(to_unsigned(3769, 12));
            when "101111" => p1 <= std_logic_vector(to_unsigned(1789, 12));
            when "110000" => p1 <= std_logic_vector(to_unsigned(2374, 12));
            when "110001" => p1 <= std_logic_vector(to_unsigned(394, 12));
            when "110010" => p1 <= std_logic_vector(to_unsigned(3005, 12));
            when "110011" => p1 <= std_logic_vector(to_unsigned(1025, 12));
            when "110100" => p1 <= std_logic_vector(to_unsigned(307, 12));
            when "110101" => p1 <= std_logic_vector(to_unsigned(2918, 12));
            when "110110" => p1 <= std_logic_vector(to_unsigned(938, 12));
            when "110111" => p1 <= std_logic_vector(to_unsigned(3549, 12));
            when "111000" => p1 <= std_logic_vector(to_unsigned(2831, 12));
            when "111001" => p1 <= std_logic_vector(to_unsigned(851, 12));
            when "111010" => p1 <= std_logic_vector(to_unsigned(3462, 12));
            when "111011" => p1 <= std_logic_vector(to_unsigned(1482, 12));
            when "111100" => p1 <= std_logic_vector(to_unsigned(764, 12));
            when "111101" => p1 <= std_logic_vector(to_unsigned(3375, 12));
            when "111110" => p1 <= std_logic_vector(to_unsigned(1395, 12));
            when "111111" => p1 <= std_logic_vector(to_unsigned(4006, 12));
            when others => p1 <= (others => '0');
        end case;
    end process;

    -- assign p0 = z_in[11:0]
    p0 <= z_in(11 downto 0);

end architecture rtl;