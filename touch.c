/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

void _ep() {
        asm volatile (
                "ldr r0, [sp]\n\t"
                "add r1, sp, #4\n\t"
                "cmp r0, #1\n\t"
                "ble exit\n\t"
                "mov r2, #0\n\t"
                "loop:\n\t"
                "ldr r3, [r1, r2, LSL #2]\n\t"
                "mov r4, #0\n\t"
                "mov r5, #0x41\n\t"
                "mov r6, #0644\n\t"
                "mov r7, #56\n\t"
                "swi 0\n\t"
                "add r2, r2, #1\n\t"
                "cmp r2, r0\n\t"
                "blt loop\n\t"
                "exit:\n\t"
                "mov r0, #0\n\t"
                "mov r7, #1\n\t"
                "swi 0\n\t"
                :
                :
                : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
        );
        for (;;);
}
