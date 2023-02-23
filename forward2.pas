Program OutputLines(output);

procedure fun1() ; forward;

procedure fun2();
begin
    write(2);
    fun1();
end;

procedure fun1();
begin
   write(1);
end;

begin
    fun2();
end.
