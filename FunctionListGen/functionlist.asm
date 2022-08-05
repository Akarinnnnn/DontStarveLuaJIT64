extrn backing:QWORD
extrn ExportFunctionCount:QWORD
_text SEGMENT
impl1 proc public 
lea rax, [ExportFunctionCount]+0
jmp rax
impl1 endp

impl2 proc public 
jmp [ExportFunctionCount]+ 8*1
impl2 endp
_text ends
END