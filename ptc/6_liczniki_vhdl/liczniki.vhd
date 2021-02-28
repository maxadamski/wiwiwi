-- Dariusz Max Adamski
-- Wtorek, 18:30
-- Nr studenta 16
-- Liczniki mod 8 i mod 47

library ieee;
use ieee.std_logic_1164.all;
entity liczniki is
	port (
		sw: in std_logic_vector(17 downto 0);
		key: in std_logic_vector(3 downto 0);
		ledr: out std_logic_vector(17 downto 0)
	);
end;

architecture structure of liczniki is
signal res: std_logic_vector(7 downto 0);
signal dir, set, del: std_logic;
begin
	-- aby przetestowac licznik modulo 8, prosze odkomentowac licznik1 i zakomentowac wszystko po nim
	--licznik1: work.mod8 port map (key(0), ledr(3 downto 0));
	
	licznik2: work.mod128rev port map (key(0), dir, set, del, "00101110", res);
	dir <= sw(0);
	del <= '1' when (dir = '1' and res = "00101111") else '0';
	set <= '1' when (dir = '0' and res = "00000000") else '0';
	ledr(7 downto 0) <= res;
end;

-- Licznik mod 8

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
entity mod8 is
	port(
		clk: in std_logic;
		q: out std_logic_vector(3 downto 0)
	);
end;

architecture behavior of mod8 is
begin
	process (clk) is
	variable x: std_logic_vector(3 downto 0) := "0000";
	begin
		if (x = "1000") then
			x := "0000";
		elsif (rising_edge(clk)) then
			x := x + 1;
		end if;
		q <= x;
	end process;
end;


-- Licznik rewersyjny mod 47
-- zrobilem fajny licznik ktory moze byc uzyty jako dowolny licznik rewersyjny mod <=128
-- jedynka na del powoduje asynchroniczne wyzerowanie licznika
-- jedynka na set przy dodatnim zboczu powoduje przypisanie data na wyjscie
-- na wejscie data wystarczy podac nasze mod - 1 i gotowe!

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
entity mod128rev is
	port(
		clk, dir, set, del: in std_logic;
		data: in std_logic_vector(7 downto 0);
		q: out std_logic_vector(7 downto 0)
	);
end;

architecture behavior of mod128rev is
begin
	process (clk, del, dir) is
	variable x: std_logic_vector(7 downto 0) := "00000000";
	begin
		if del = '1' then
			x := "00000000";
		elsif rising_edge(clk) then
			if set = '1' then
				x := data;
			elsif dir = '1' then
				x := x + 1;
			else
				x := x - 1;
			end if;
		end if;
		q <= x;
	end process;
end;
