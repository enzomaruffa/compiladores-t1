Program OutputLines(output);

procedure fun1() ; forward;

procedure fun2();
begin
    writeln('Inside fun2. Calling fun1()');
    fun1();
end;

procedure fun1();
begin
   writeln('Inside fun1');
end;

begin
    fun2();
end.
