-- Dariusz Max Adamski
-- Wtorek, 18:30
-- E, F, G, H, I

library ieee;
use ieee.std_logic_1164.all;
entity reklama is
	port (
		sw: in std_logic_vector(17 downto 0);
		key: in std_logic_vector(0 to 3);
		hex0, hex1, hex2, hex3, hex4, hex5, hex6, hex7: out std_logic_vector(0 to 6)
	);
end;

architecture structure of reklama is
constant space: std_logic_vector(2 downto 0) := "000";
signal chr0, chr1, chr2, chr3, chr4, chr5, chr6, chr7: std_logic_vector(2 downto 0);
signal move: std_logic_vector(2 downto 0);
begin
	cnt0: work.mod8 port map (sw(17), move);
	
	-- wygenerowane przez skrypt 'reklama.py'
	
	mux0: work.mux8to1 port map (move, sw(14 downto 12), sw(11 downto 9), sw(8 downto 6), sw(5 downto 3), sw(2 downto 0), space, space, space, chr0);
	mux1: work.mux8to1 port map (move, sw(11 downto 9), sw(8 downto 6), sw(5 downto 3), sw(2 downto 0), space, space, space, sw(14 downto 12), chr1);
	mux2: work.mux8to1 port map (move, sw(8 downto 6), sw(5 downto 3), sw(2 downto 0), space, space, space, sw(14 downto 12), sw(11 downto 9), chr2);
	mux3: work.mux8to1 port map (move, sw(5 downto 3), sw(2 downto 0), space, space, space, sw(14 downto 12), sw(11 downto 9), sw(8 downto 6), chr3);
	mux4: work.mux8to1 port map (move, sw(2 downto 0), space, space, space, sw(14 downto 12), sw(11 downto 9), sw(8 downto 6), sw(5 downto 3), chr4);
	mux5: work.mux8to1 port map (move, space, space, space, sw(14 downto 12), sw(11 downto 9), sw(8 downto 6), sw(5 downto 3), sw(2 downto 0), chr5);
	mux6: work.mux8to1 port map (move, space, space, sw(14 downto 12), sw(11 downto 9), sw(8 downto 6), sw(5 downto 3), sw(2 downto 0), space, chr6);
	mux7: work.mux8to1 port map (move, space, sw(14 downto 12), sw(11 downto 9), sw(8 downto 6), sw(5 downto 3), sw(2 downto 0), space, space, chr7);

	tco0: work.chr7seg port map (chr0, hex0);
	tco1: work.chr7seg port map (chr1, hex1);
	tco2: work.chr7seg port map (chr2, hex2);
	tco3: work.chr7seg port map (chr3, hex3);
	tco4: work.chr7seg port map (chr4, hex4);
	tco5: work.chr7seg port map (chr5, hex5);
	tco6: work.chr7seg port map (chr6, hex6);
	tco7: work.chr7seg port map (chr7, hex7);
end;

-- Multiplekser 8 do 1

library ieee;
use ieee.std_logic_1164.all;
entity mux8to1 is
	port (
		mux_sel, u0, u1, u2, u3, u4, u5, u6, u7: in std_logic_vector(2 downto 0);
		mux_out: out std_logic_vector(2 downto 0)
	);
end;

architecture structure of mux8to1 is
begin
	with mux_sel select mux_out <=
		u0 when "000",
		u1 when "001",
		u2 when "010",
		u3 when "011",
		u4 when "100",
		u5 when "101",
		u6 when "110",
		u7 when "111";
end;

-- Transkoder liczby 3bitowej do kodu wyswietlacza 7-segmentowego

library ieee;
use ieee.std_logic_1164.all;
entity chr7seg is
	port (
		tco_chr: in std_logic_vector(2 downto 0);
		tco_out: out std_logic_vector(0 to 6)
	);
end;

architecture structure of chr7seg is
begin
	with tco_chr select tco_out <=
		"1111111" when "000", -- " "
		"0110000" when "001", -- "E"
		"0111000" when "010", -- "F"
		"0100000" when "011", -- "G"
		"1001000" when "100", -- "H"
		"1111001" when "101", -- "I"
		"1111111" when others;
end;

-- Licznik mod 8, do automatycznego przesuwania napisu

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
entity mod8 is
    port (
		clk: in std_logic;
		q: out std_logic_vector (2 downto 0)
	);
end;

architecture behavioral of mod8 is
begin
	process (clk)
	variable x: std_logic_vector(2 downto 0);
	begin
		if (clk'event and clk = '1') then
			if (x = "111") then
				x := "000";
			else
				x := x + 1;
			end if;
		end if;
		q <= x;
	end process;
end;
