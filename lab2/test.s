    .arch armv5t
    .text
    .align 2
    .section .rodata
    .align 2 
_str0:
    .ascii "%d"
    .align 2
@_str2:
@    .ascii "test %d\n\0"
@    .align 2
_str1:
    .ascii "%d\n\0"
    .text
    .align 2
    .global main

main:
    push {fp,lr}
    add fp,sp,#12

    mov r1,sp
    ldr r0,_bride
    bl __isoc99_scanf @ scanf("%d",&n)
    movs r2,#2
    str r2,[sp,#4] @r2=i
    movs r2,#1
    str r2,[sp,#8] @r1=f
    b .L0

.L1:
    
    ldr r3,[sp,#8]
    ldr r2,[sp,#4]
    mul r3,r2,r3
    str r3,[sp,#8]
    adds r2,r2,#1
    str r2,[sp,#4]
.L0:    
    ldr r3,[sp]
    ldr r2,[sp,#4]
    cmp r2,r3
    ble .L1
    ldr r1,[sp,#8]
    ldr r0,_bride+4
    bl printf
    mov r0,#0
    pop {fp,pc}


_bride:
    .word _str0
    .word _str1
@    .word _str2
    .section .note.GNU-stack,"",%progbits
