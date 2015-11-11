Library ieee;
Use ieee.std_logic_1164.all;
Entity Mult_4by4 is
port(q, m : in std_logic_vector(3 downto 0);
P : out std_logic_vector(7 downto 0));
End Entity Mult_4by4;
Architecture behaviour of Mult_4by4 is
signal C_Top1, C_Top2, C_Top3, PP1 : std_logic;
signal PP_Top1, PP_Top2, PP_Top3 : std_logic;
signal C_Middle1, C_Middle2, C_Middle3, PP2 : std_logic;
signal PP_Middle1, PP_Middle2, PP_Middle3 : std_logic;
signal C_Bottom1, C_Bottom2, C_Bottom3 : std_logic;
Begin

P(0) <= m(0) and q(0);

Top1 : Entity work.TopBlock 
port map(mk_1=>m(1), mk=>m(0), q0=>q(0), q1=>q(1), cin=>'0', PP=>P(1), cout=>C_Top1);
Top2 : Entity work.TopBlock
port map(mk_1=>m(2), mk=>m(1), q0=>q(0), q1=>q(1), cin=>C_Top1, PP=>PP_Top1, cout=>C_Top2);
Top3 : Entity work.TopBlock 
port map(mk_1=>m(3), mk=>m(2), q0=>q(0), q1=>q(1), cin=>C_Top2, PP=>PP_Top2, cout=>C_Top3);
Top4 : Entity work.TopBlock 
port map(mk_1=>'0', mk=>m(3), q0=>q(0), q1=>q(1), cin=>C_Top3, PP=>PP_Top3, cout=>PP1);

Middle1 : Entity work.LowerBlock 
port map(PPi=>PP_Top1, mk=>m(0), qj=>q(2), cin=>'0', PP=>P(2), cout=>C_Middle1); 
Middle2 : Entity work.LowerBlock 
port map(PPi=>PP_Top2, mk=>m(1), qj=>q(2), cin=>C_Middle1, PP=>PP_Middle1, cout=>C_Middle2);
Middle3 : Entity work.LowerBlock 
port map(PPi=>PP_Top3, mk=>m(2), qj=>q(2), cin=>C_Middle2, PP=>PP_Middle2, cout=>C_Middle3);
Middle4 : Entity work.LowerBlock 
port map(PPi=>PP1, mk=>m(3), qj=>q(2), cin=>C_Middle3, PP=>PP_Middle3, cout=>PP2);

Bottom1 : Entity work.LowerBlock 
port map(PPi=>PP_Middle1, mk=>m(0), qj=>q(3), cin=>'0', PP=>P(3), cout=>C_Bottom1);
Bottom2 : Entity work.LowerBlock 
port map(PPi=>PP_Middle2, mk=>m(1), qj=>q(3), cin=>C_Bottom1, PP=>P(4), cout=>C_Bottom2);
Bottom3 : Entity work.LowerBlock 
port map(PPi=>PP_Middle3, mk=>m(2), qj=>q(3), cin=>C_Bottom2, PP=>P(5), cout=>C_Bottom3);
Bottom4 : Entity work.LowerBlock
port map(PPi=>PP2, mk=>m(3), qj=>q(3), cin=>C_Bottom3, PP=>P(6), cout=>P(7));

End behaviour;