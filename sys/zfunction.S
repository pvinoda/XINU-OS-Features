.global zfunction        

zfunction:
    mov    0x4(%esp), %eax   
    and    $0xfffff00f, %eax 
    shl    $0x8, %eax        
    or     $0xff, %al        
    ret                     