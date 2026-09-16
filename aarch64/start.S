.global _start
.text
_start:
        ldr x0, [sp]
        add x1, sp, #8
        bl _ep
