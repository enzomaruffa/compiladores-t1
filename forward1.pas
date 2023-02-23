Program Maximum_Minimum_Number(output);

var 
    res: integer;

function max(num1, num2: integer): integer; forward;

function min(num1, num2: integer): integer;
var
   result: integer;

begin
   if (num1 > num2) then
      result := num2
   else
      result := num1;
   min := result;
end;


function max(num1, num2: integer): integer;
var
   result: integer;

begin
   if (num1 > num2) then
      result := num1
   else
      result := num2;
   max := result;
end;

begin
  res := max(5,10);
  write(res);
end.
