.section .text
.global _start
.global exit
.extern main

_start:
ldr r0, [sp]
add r1, sp, #4
bl main
bl exit
1:  b 1b

exit:
mov r7, #1
swi 0
b 1b
