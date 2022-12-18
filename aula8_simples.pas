program comandoWhile;
var n, k: integer;
    f1, f2, f3:integer;
begin
   f1:=0; f2:=1; k:=1;
   while (k <= n) do
   begin
      f3:=f2+f1;
      f1:=f2;
      f2:=f3;
      k:=k+1
   end;
   end.
