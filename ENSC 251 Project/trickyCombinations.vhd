signal CLK : std_logic;
signal signalOne : std_logic_vector(0 to 3):=
x"A";
signal signalTwo :std_logic_vector(3 downto 0):=
"0011";
signal signalThree: std_logic_vector(3 downto 0):= 	('0' & "011");
type vecOfVec is array (Natural range <>) of std_logic_vector(0 to 1);
signal signalFour : vecOfVec(2 downto 0);
signal 
signal R : someRecordType;
begin
-- text
some_component : component_name
port map (
	CLK => CLK,
	signalOne =>signalOne, --a comment
	signalTwo=> signalTwo
); 

process(signalOne,signalTwo)
	variable C : integer :=0;
begin
	C :=5**2;
	if (signalOne < signalTwo and signalTwo'length = 4 and signalTwo(1) = '0') then signalOne <= signalThree; elsif (signalOne < signalThree or signalOne /= signalOne or R.someMember) signalOne <= signalTwo else	
		signalOne <= '0' & '1' & signalTwo(1 downto 0);
	        signalTwo <= (others => '0');  
	        --signalTwo <= (others => '1');
		if (signalThree'
length =4)then
		signalThree	 <=B"1111";
end if;
		signalFour(1) <= '1' & '0';
	end if;--abc
end process;
end;
