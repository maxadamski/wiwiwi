-- Dariusz Max Adamski
-- Wtorek, 18:30

library ieee;
use ieee.std_logic_1164.all;
entity mnozarka is
	port (
		sw: in std_logic_vector(17 downto 0);
		key: in std_logic_vector(3 downto 0);
		hex0, hex1, hex2, hex3, hex4, hex5, hex6, hex7: out std_logic_vector(0 to 6);
		ledr: out std_logic_vector(17 downto 0)
	);
end;

architecture structure of mnozarka is
begin
	mul0: work.multi port map (sw(17 downto 14), sw(1 downto 0), ledr(5 downto 0));
end;

-- Sumator 3bit

library ieee;
use ieee.std_logic_1164.all;
entity adder is
	port (
		a, b, c: in std_logic;
		carry, sum: out std_logic
	);
end;

architecture structure of adder is
begin
	sum <= a xor b xor c;
	carry <= (b and c) or (a and c) or (a and b);
end;

-- Mnozarka 4x2bit

library ieee;
use ieee.std_logic_1164.all;
entity multi is
	port (
		a: in std_logic_vector(3 downto 0);
		b: in std_logic_vector(1 downto 0);
		mul: out std_logic_vector(5 downto 0)
	);
end;

architecture structure of multi is
type array2d is array (0 to 1, 0 to 3) of std_logic;
signal c, s: array2d;
begin
	add00: work.adder port map (a(1) and b(0), a(0) and b(1), '0'    , c(0, 0), s(0, 0));
	add01: work.adder port map (a(2) and b(0), a(1) and b(1), c(0, 0), c(0, 1), s(0, 1));
	add02: work.adder port map (a(3) and b(0), a(2) and b(1), c(0, 1), c(0, 2), s(0, 2));
	add03: work.adder port map ('0'          , a(3) and b(1), c(0, 2), c(0, 3), s(0, 3));
	
	mul(0) <= a(0) and b(0);
	mul(1) <= s(0, 0);
	mul(2) <= s(0, 1);
	mul(3) <= s(0, 2);
	mul(4) <= s(0, 3);
	mul(5) <= c(0, 3);
end;
