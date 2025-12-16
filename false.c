/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

__attribute__((naked, section(".text.startup")))

void _ep(void) {
	asm volatile (
		"mov r0, #1\n"
		"mov r7, #1\n"
		"svc 0\n"
	);
}
