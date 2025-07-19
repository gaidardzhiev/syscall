# Minimalist *nix Utilities Using Raw Syscalls and Inline Assembly (ARMv8l 32-bit)

This project implements a collection of classic `*nix` utilities written in C and ARMv8l 32-bit inline assembly that **bypass the C standard library (`libc`)** entirely, invoking system calls directly. The goal of this project is to deeply explore and understand how computers truly operate at the hardware level by writing software that interacts directly with the Linux kernel via raw system calls and ARMv8l 32bit assembly bypassing all high level abstractions such as libc. This approach teaches the essential principles of low level programming, reveals how operating systems manage processes and resources, and exemplifies how software should be programmed when maximum control, efficiency, and minimal abstraction are required.

---

## Overview

Each tool in this project uses **direct kernel system calls** by crafting syscall numbers and arguments inline, interfacing with the kernel via the ARM specific `svc #0` instruction. This is done without any libc wrappers or stdio buffering, often using a minimal syscall bridge and custom startup code in assembly.

The tools include:

| Program   | Description                                                                                         | Key Technique                                        |
|-----------|-------------------------------------------------------------------------------------------------|-----------------------------------------------------|
| `cat.c`  | Concatenates files and writes to stdout using low-level syscalls via `syscall()` from glibc       | `syscall()` call from glibc but only for syscall NR |
| `echo.c` | Writes command line arguments to stdout using direct `svc` instructions bypassing libc            | Inline ARM assembly with `svc #0` syscall           |
| `sleep.c`| Suspends execution for specified seconds by directly invoking `nanosleep` syscall inline          | Inline assembly syscall without libc                |
| `false.c`| Immediately exits with status code 1 by invoking `exit` syscall using inline assembly             | Raw exit syscall with status 1                       |
| `true.c` | Same as `false.c` but exits with status code 0                                                    | Raw exit syscall with status 0                       |
| `bridge.c`| Provides a bridge interface allowing POSIX shell scripts to invoke any syscall by number & args  | Raw syscall interface bridging user input to kernel |
| `tty.c`  | Prints the filename of the terminal connected to STDIN via inline assembly                         | Inline assembly syscall reading `/proc/self/fd/0`   |
| `shell.c`| Minimal shell loop implementing `read`, `fork`, `execve`, and `wait` syscalls with assembly       | Inline assembly minimal shell loop                   |
| `crt0.s` | ARMv8l 32-bit minimal assembly startup code                                                      | Extract `argc` / `argv` from stack, call `main()`, exit syscall |

---

## Motivation

- **Bypass standard libc wrappers** for syscalls for minimal overhead and maximum control.
- Learn low-level Linux ARM syscall conventions.
- Understand mixing inline assembly with C for syscall invocation.
- Create minimalist tools demonstrating syscall usage, process lifecycle, and shell basics.
- Targeted at ARMv8l 32-bit Linux environments.

---
