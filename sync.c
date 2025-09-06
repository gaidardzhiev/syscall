/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

void _ep(void) __attribute__((noreturn));
void _ep(void) {
	__asm__ volatile (
		"mov r7, #36\n\t"
		"svc 0\n\t"
		"mov r7, #1\n\t"
		"mov r0, #0\n\t"
		"svc 0"
		:
		:
		: "r7", "r0"
	);
	__builtin_unreachable();
}
