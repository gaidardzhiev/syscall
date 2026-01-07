.global _start
.text

_start:
ldr	r0, [sp]
add	r1, sp, #4
b	_ep
