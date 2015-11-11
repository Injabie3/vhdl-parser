Library ieee;
Use ieee.std_logic_1164.all;
Use ieee.numeric_std.all;

Entity LB08 is
	PORT ( KEY, SW:	IN std_logic_vector (3 downto 0);
			CLOCK_50:	IN std_logic;
			HEX2, HEX1, HEX0:	OUT std_logic_vector (6 downto 0);
			LEDR:	OUT std_logic_vector (4 downto 0);
			LEDG:	OUT std_logic_vector (2 downto 0));
			B"LJKSDHFKLJSHDFKLJ";
			O"IERILKJSFK";
			X"jskldfjklsd18282923490";
			b"822hjks9__28ks";
			o"82382kisd";
			x"8282jdjkd";
END ENTITY LB08;

ARCHITECTURE TestSys OF LB08 IS
	SIGNAL clock, enable: std_logic;

BEGIN

prescale: ENTITY work.PreScale PORT MAP(
			CLOCK_50, clock);
			
System: ENTITY  work.System(Basic) PORT MAP (
			KEY(3), clock, SW(3 downto 0),
			LEDG(2), LEDG(1),enable);
			
Alarm:ENTITY work.Alarm PORT MAP (
			enable, clock,
			HEX0, HEX1, HEX2);
			
LEDG(0) <= enable;
LEDR(4) <= clock;
LEDR (3 downto 0)<= SW;

END ARCHITECTURE TestSys;