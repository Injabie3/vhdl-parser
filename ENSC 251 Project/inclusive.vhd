library IEEE;
use IEEE.Std_logic_1164.all;
entity EntName is
  port (P1, P2: in Std_logic;
        P3: out Std_logic_vector(7 downto 0));
end EntName;
architecture ArchName of EntName is
  component CompName
    port (P1: in  Std_logic;
          P2: out Std_logic);
  end component;
  signal SignalName, SignalName2: Std_logic := 'U';
begin
  P: process (P1,P2,P3) -- 
Either sensitivity list or wait statements
    variable VariableName, VarName2: Std_logic := 'U';
  begin
    SignalName <= Expression after Delay;
    VariableName := Expression;
    ProcedureCall(Param1, Param2, Param3);
    wait for Delay;
    wait until Condition;
    wait;
    if Condition then
      -- 
sequential statements
    elsif Condition then
      -- 
sequential statements
    else
      -- 
sequential statements
    end if;
    case Selection is
    when Choice1 =>
      -- 
sequential statements
    when Choice2 | Choice3 =>
      -- 
sequential statements
    when others =>
      -- 
sequential statements
    end case;
    for I in A'Range loop
      -- 
sequential statements
    end loop;
  end process P;
  SignalName <= Expr1 when Condition else Expr2;
  InstanceLabel: CompName port map (S1, S2);
  L2: CompName port map (P1 => S1, P2 => S2);
  G1: for I in A'Range generate
    -- 
concurrent statements
  end generate G1;
end ArchName;

package PackName is
  type Enum is (E0, E1, E2, E3);
  subtype Int is Integer range 0 to 15;
  type Mem is array (Integer range <>) of
              Std_logic_vector(7 downto 0);
  subtype Vec is Std_logic_vector(7 downto 0);
  constant C1: Int := 8;
  constant C2: Mem(0 to 63) := (others => "11111111");
  procedure ProcName (ConstParam: Std_logic;
                      VarParam: out Std_logic;
               signal SigParam: inout Std_logic);
  function "+" (L, R: Std_logic_vector)
                      return Std_logic_vector;
end PackName;
package body PackName is
  procedure ProcName (ConstParam: Std_logic;
                      VarParam: out Std_logic;
               signal SigParam: inout Std_logic) is
    -- 
declarations
  begin
    -- 
sequential statements
  end ProcName;
  function "+" (L, R: Std_logic_vector)
                      return Std_logic_vector is
    -- 
declarations
  begin
    -- 
sequential statements
    return Expression;
  end "+";
end PackName;
configuration ConfigName of EntityName is
  for ArchitectureName
    for Instances: ComponentName
      use LibraryName.EntityName(ArchName);
    end for;
  end for;
end ConfigName;