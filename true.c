__attribute__((noreturn)) void _ep(void) {
	__asm__ volatile (
		"mov r0, #0\n"
		"mov r7, #1\n"
		"swi 0\n"
	);
	for (;;) { }
}
