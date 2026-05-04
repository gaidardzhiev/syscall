Here's a full rewrite proposal — read it through before we commit anything:

---

# Minimalist \*nix Utilities Using Raw Syscalls and Inline Assembly (ARMv8l 32-bit)

Straight kernel calls. No libc. No wrappers. No mercy.

Each tool in this project talks directly to the Linux kernel via raw `swi #0` inline assembly, loading syscall numbers into `r7` and arguments into `r0`–`r2` the way the ARM EABI intended — no `stdio`, no `glibc`, no training wheels. Custom startup assembly handles `argc`/`argv` extraction so not even `crt0` is borrowed.

The goal is simple: understand what the machine actually does when software runs, by writing software that has nowhere to hide.

---

## Tools

- `cat.c` — reads files and dumps them to stdout via raw `read(3)`/`write(4)` `swi #0` calls, `r7` loaded by hand, no `fopen` in sight

- `echo.c` — writes arguments to stdout with a single `write(4)` syscall, no buffering, no formatting layer, just bytes hitting the terminal

- `sleep.c` — suspends execution by invoking `nanosleep(162)` directly with a hand-built `timespec` on the stack, kernel does the waiting, libc does nothing

- `true.c` — calls `exit(1)` syscall with status `0` and gets out. one instruction. done.

- `false.c` — same as `true.c` but exits `1`. existence is suffering, status confirmed.

- `bridge.c` — exposes raw syscalls to POSIX shell scripts by number and arguments, the escape hatch when the shell can't reach the kernel itself

- `tty.c` — resolves the terminal name on stdin via `readlink(85)` over `/proc/self/fd/0`, prints it raw to stdout

- `shell.c` — minimalist shell: read, fork, exec, wait, repeat — pure `swi #0` throughout, no readline, no history, no regrets

- `crt0.s` — ARMv8l 32-bit startup stub in raw assembly, pulls `argc` and `argv` off the stack, calls `_ep()`, invokes `exit(1)` with the return value, the kernel hands control here first

- `id.c` — resolves uid/gid/groups via `getuid(24)`, `getgid(47)`, `getgroups(80)`, formats and writes without touching libc identity machinery

- `clear.c` — blasts the ANSI clear sequence directly to stdout fd `1` via `write(4)`, no terminfo, no ncurses, two bytes and a dream

- `pwd.c` — `getcwd(183)` straight into a 4k stack buffer, `strlen` by hand, dumped to stdout via `write(4)`, pure `r7` voodoo

- `uname.c` — fills a `utsname` struct via syscall `122`, prints fields to stdout, no `uname()` wrapper, just the raw kernel truth

- `yes.c` — infinite `write(4)` loop to stdout, prints the argument or `y` forever until something kills it, and it will not stop on its own

- `wc.c` — counts newlines, words and bytes via `read(3)` into a stack buffer, manual state machine, no `fscanf`, no mercy for whitespace

- `kill.c` — sends signals by PID via `kill(37)` syscall, parses `SIGTERM`, `-N`, `-s SIGNAL` and `-SIGNAME` by hand with zero libc signal name lookup

- `test.c` — evaluates POSIX test expressions via `swi #0` syscalls `write(4)`, `access(33)`, `stat(195)`, supports string, integer and file tests with logical operators `!`, `-a`, `-o`, invokable as `[`

- `mkdir.c` — creates directories via `mkdir(39)` syscall with explicit mode, handles multiple paths, no `mkdirp` safety net

- `expr.c` — evaluates POSIX arithmetic and string expressions via `write(4)` syscall, supports `+ - * / %`, comparisons, boolean `| &` and `length`/`substr`/`index`, integer division via portable shift-subtract since `__aeabi_idiv` isn't invited

---

## Motivation

The kernel interface is always there, one `swi #0` away. libc is just a polite suggestion. This project strips that layer off to see what's underneath — how processes start, how the kernel moves bytes, how a shell actually works when you build one yourself from `fork(2)` and `execve(11)` up. Every tool here is a worked example in that philosophy.

---

## Verification

This project includes a comprehensive verification suite (`verify.sh`) that tests each utility's correctness against standard `*nix` behavior. No frameworks — just a POSIX shell script that builds on demand, runs precise behavioral tests and reports `PASS`/`FAIL` with diagnostic output.

A wise man once said *"Testing shows the presence, not the absence of bugs"* — this suite catches the essential deviations. Each test function isolates one observable property: exit codes, output fidelity, timing, syscall traces. Key principles:

- **Behavioral equivalence** — stdout, stderr and exit status compared against system reference behavior
- **No libc dependency** — raw syscall paths confirmed by `strace` where needed
- **Precision timing** — `sleep` measured via epoch seconds, `yes` via output rate
- **Self contained** — builds missing binaries via `make`, cleans temporaries, pure POSIX shell

Run `./verify.sh` to validate the purity.

---

## Get Syscalls

`get_syscalls.sh` uses `cpp -dM` to dump all preprocessor macros from `sys/syscall.h`, detects the platform prefix (`__NR_` on Linux, `SYS_` elsewhere) via `uname`, strips it with `sed` and sorts alphabetically to stdout.

---

## License

Copyright (C) 2025-2026 Ivan Gaydardzhiev. Licensed under GPL-3.0-only — see [COPYING](./COPYING) for details.

---

The main changes: motivation collapsed from four redundant bullets into one tight paragraph, struck-through text gone, tool entries unified in voice — each one leads with what syscall does the work and what's notably absent. `true`/`false` got their personality back. Want me to adjust the tone anywhere or is this ready to drop in?
