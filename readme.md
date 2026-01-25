# Minimalist *nix Utilities Using Raw Syscalls and Inline Assembly (ARMv8l 32-bit)

This project implements a collection of classic `*nix` utilities written in C and ARMv8l 32 bit inline assembly that **bypass the C standard library (`libc`)** entirely, invoking system calls directly. The goal of this project is to deeply explore and understand how computers truly operate at the lower level by writing software that interacts directly with the Linux kernel via raw system calls and ARMv8l 32 bit assembly bypassing all high level abstractions such as `libc`. This approach teaches the essential principles of low level programming, reveals how operating systems manage processes and resources, and exemplifies how software **should be programmed** when maximum control, efficiency, and minimal abstraction are required.

---

## Overview

Each tool in this project uses **direct kernel system calls** by crafting syscall numbers and arguments inline, interfacing with the kernel via the ARM specific `svc #0` instruction. This is done without any `libc` wrappers or `stdio` buffering, often using a custom startup code in assembly.

The tools include:

- `cat.c` - concatenate files and print to stdout using pure raw ARM `swi #0` syscalls in `r7/r0-r2` registers ~~low level system calls via `syscall()` provided by `glibc`~~

- `echo.c` - write arguments to stdout using low level system calls bypassing `libc` via direct `svc` instructions to the kernel

- `sleep.c` - suspend execution for a specified number of seconds by directly invoking the `nanosleep` syscall using inline assembly without relying on `libc`

- `false.c` - do nothing unsuccessfully by directly invoking the kernel `exit` syscall with status `1` using inline assembly and bypassing `libc` entirely

- `true.c` - do nothing successfully by directly invoking the kernel `exit` syscall with status `0` using inline assembly and bypassing `libc` entirely

- `bridge.c` - bridge between raw syscalls and the POSIX shell allowing scripts to access any syscall by number and arguments

- `tty.c` - print the file name of the terminal connected to standard input using inline assembly

- `shell.c` - minimalist shell that cycles reading, forking, executing and waiting using only raw syscalls and inline assembly ~~(commands must be given as absolute paths)~~

- `crt0.s` - minimal armv8l 32 bit assembly startup code that initializes the process by extracting `argc` and `argv` from the stack, calls `main()` and then invokes the `exit` syscall with main's return value as the process exit code

- `id.c` - print user, group and system identity by invoking only kernel syscalls directly with ARM EABI inline assembly, bypassing all standard C library functions

- `clear.c` - clear the terminal screen by writing `ANSI` escape sequences directly to stdout using raw `Linux` syscalls with inline assembly bypassing `libc` entirely

- `pwd.c` - print current dir from kernel with raw arm syscall no `libc` crap (straight `svc #0` `getcwd(183)`, `4k buf`, `strlen`, `dump stdout`, pure `r7` voodoo)

- `uname.c` - print kernel `utsname` via raw arm `swi #0` syscall(122), no `libc` crutches

- `yes.c` - print string infinitely to stdout via raw arm `swi #0` write(4) syscall loop ~~(argument parsing broken despite r0/r1 capture)~~

- `wc.c` - print newline, word, and byte counts for each file using pure raw kernel syscalls

---

## Motivation

- **Bypass standard libc wrappers** for syscalls for minimal overhead and maximum control.
- Learn low level Linux ARMV8L syscall conventions.
- Understand mixing inline assembly with C for syscall invocation.
- Create minimalist tools demonstrating syscall usage, process lifecycle, and shell basics...

---

## License

This project is provided under the GPL3 License.

---

## Verification

This project includes a comprehensive verification suite (`verify.sh`) that rigorously tests each utility's functional correctness against standard `*nix` behavior. Following the minimalist philosophy, verification eschews complex frameworks for a simple POSIX shell script that compiles on demand, executes precise behavioral tests, and reports `PASS`/`FAIL` status with diagnostic output.

The suite embodies the `*nix` tradition of **do one thing well:** each test function isolates a single observable property (exit codes, output fidelity, timing, syscall traces) and performs a direct comparison against reference implementations or kernel truths. A wise man once observed "Testing shows the presence, not the absence of bugs." yet this approach catches the essential deviations from expected behavior.

Key verification principles:
- **Behavioral equivalence**: Each tools `stdout`, `stderr` and `exit` status are compared against system equivalents `[ "${x}" = "${y}" ] && 0 || 1`
- **No libc dependency**: Tests confirm raw syscall paths by `strace` inspection where needed (`sync`, `clear`).
- **Precision timing**: `sleep` measured via epoch seconds; `yes` via output rate parity.
- **Self contained**: Builds missing binaries via `make`, cleans temporaries, portable POSIX shell syntax.

Just run `./verify.sh` to validate the purity.

## Get syscalls

The `get_syscalls.sh` script uses `cpp -dM` to dump all preprocessor macros from `sys/syscall.h`. It detects the platform prefix (`__NR_` on Linux, `SYS_` elsewhere) via `uname`, then `sed` strips the prefix, and `sort` orders alphabetically, printing the result to `stdout`.
