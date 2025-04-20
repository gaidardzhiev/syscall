__attribute__((naked, section(".text.startup")))

void _ep(void) {
	__asm__ volatile (
		"mov r0, #1\n"
		"mov r7, #1\n"
		"svc 0\n"
	);
}
