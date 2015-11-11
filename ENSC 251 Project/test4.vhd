<tt>LIBRARY IEEE;        
USE IEEE.std_logic_1164.ALL;
USE IEEE.numeric_std.ALL;
         
entity tap_filter is
port(    Clock : in std_logic; --clock signal
        Xin : in signed(7 downto 0); --input signal
    Yout : out signed(15 downto 0) --filter output 
    );
end tap_filter;
         
architecture Behavioural of tap_filter is
         
component DFF_Stuff is
port(
        Q : out signed(15 downto 0); --connected to the adder for output
        Clock : in std_logic; --clock input
        D : in signed(15 downto 0) --data input from MCM block
    );
end component;
-- signals for 4tap
signal h0,h1,h2,h3 : signed(7 downto 0) := (others => '0');
signal mcm0,mcm1,mcm2,mcm3,addout1,addout2,addout3 : signed(15 downto 0) := (others => '0');
signal q1,q2,q3 : signed(15 downto 0) := (others => '0');
         
begin
         
--initialize the filter coefficients
--where H = [-2 -1 3 4]
h0 <= to_signed(-2,8);
h1 <= to_signed(-1,8);
h2 <= to_signed(3,8);
h3 <= to_signed(4,8);
--mcm: the multiple constant multiplications
mcm0 <= h0*Xin;
mcm1 <= h1*Xin;
mcm2 <= h2*Xin;
mcm3 <= h3*Xin;
--le adders!
addout1 <= q1+mcm2;
addout2 <= q2+mcm1;
addout3 <= q3+mcm0;
--creating flipflops for making a delay
dff1 : DFF_Stuff port map(q1,Clock,mcm3);
dff2 : DFF_Stuff port map(q2,Clock,addout1);
dff3 : DFF_Stuff port map(q3,Clock,addout2);
--producing output at the rising edge of the clock cycle
process(Clock)
begin
    if(rising_edge(Clock)) then
        Yout <= addout3;
    end if;
end process;
         
end Behavioural;</tt>