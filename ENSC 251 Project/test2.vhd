library ieee;               
use ieee.numeric_std.all;  
use ieee.std_logic_1164.all;  

Entity IncStage1 is
  port( X1, Cin:in std_logic;    
        Cout, S1: out std_logic);
End Entity IncStage1;
Architecture stuff of IncStage1 is
Begin
  S1 <= X1 XOR Cin after 4 ns;    
  Cout <= Cin AND X1 after 4 ns; 
End Architecture stuff;