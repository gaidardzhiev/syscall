# Minimalist *nix Utilities Using Raw Syscalls and Inline Assembly (ARMv8l 32-bit)

This project implements a collection of classic `*nix` utilities written in C and ARMv8l 32-bit inline assembly that **bypass the C standard library (`libc`)** entirely, invoking system calls directly. The goal of this project is to deeply explore and understand how computers truly operate at the hardware level by writing software that interacts directly with the Linux kernel via raw system calls and ARMv8l 32bit assembly bypassing all high level abstractions such as libc. This approach teaches the essential principles of low level programming, reveals how operating systems manage processes and resources, and exemplifies how software should be programmed when maximum control, efficiency, and minimal abstraction are required.

---

## Overview

Each tool in this project uses **direct kernel system calls** by crafting syscall numbers and arguments inline, interfacing with the kernel via the ARM specific `svc #0` instruction. This is done without any libc wrappers or stdio buffering, often using a minimal syscall bridge and custom startup code in assembly.

The tools include:

- `cat.c` - concatenate files and print to stdout using low level system calls via `syscall()` provided by `glibc`

- `echo.c` - write arguments to stdout using low level system calls bypassing libc via direct svc instructions to the kernel

- sleep.c - suspend execution for a specified number of seconds by directly invoking the nanosleep syscall using inline assembly without relying on libc

- false.c - do nothing unsuccessfully by directly invoking the kernel exit syscall with status 1 using inline assembly and bypassing libc entirely

- true.c - do nothing successfully by directly invoking the kernel exit syscall with status 0 using inline assembly and bypassing libc entirely

- bridge.c - bridge between raw syscalls and the POSIX shell allowing scripts to access any syscall by number and arguments

- tty.c - print the file name of the terminal connected to standard input using inline assembly

- shell.c - minimalist shell that cycles reading, forking, executing and waiting using only raw syscalls and inline assembly

- crt0.s - minimal armv8l 32bit assembly startup code that initializes the process by extracting argc and argv from the stack, calls main and then invokes the exit syscall with main's return value as the process exit code


---

## Motivation

- **Bypass standard libc wrappers** for syscalls for minimal overhead and maximum control.
- Learn low-level Linux ARM syscall conventions.
- Understand mixing inline assembly with C for syscall invocation.
- Create minimalist tools demonstrating syscall usage, process lifecycle, and shell basics.
- Targeted at ARMv8l 32-bit Linux environments.

---

## License

This project is provided under the GPL3 License.


