/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

const char c[7] = {
	27,
	'[',
	'2',
	'J',
	27,
	'[',
	'H'
};

void _ep() {
	asm volatile (
		"mov r0, #1\n\t"
		"mov r1, %0\n\t"
		"mov r2, #7\n\t"
		"mov r7, %1\n\t"
		"swi 0\n\t"
		"mov r0, #0\n\t"
		"mov r7, %2\n\t"
		"swi 0\n\t"
		:
		: "r" (c), "I" (4), "I" (1)
		: "r0", "r1", "r2", "r7"
	);
	for (;;);
}
