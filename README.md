# libsclog

`libsclog` formats a supplied syscall number, arguments, result, and errno as a strace-style log without tracing a live process.

## Layout

- `include/sclog.h`: public C/C++ API.
- `strace/`: strace Git submodule.
- `tests/sclog_test.c`: API and decoding smoke test.
- `build/`: out-of-tree generated files and build outputs.

## Build

Requirements include a C compiler, GNU Make, Autoconf, Automake, and the normal strace build prerequisites.

```sh
git submodule update --init
make
```

The library is written to `build/libsclog.a`. The default configuration builds the native syscall personality and disables optional stack-trace and SELinux integrations, keeping the static library dependency set stable.

To run the smoke test:

```sh
make test
```

`build.sh` is a parallel-build convenience wrapper. Set `JOBS` to control its job count:

```sh
JOBS=8 ./build.sh
```

## API

Initialize the decoder once, assign an output stream, then submit the entering and exiting halves of each syscall:

```c
#include <sclog.h>

intptr_t args[] = { /* syscall arguments */ };

init_sclog();
set_log_file(0, output);
log_syscall_with_index(0, index, syscall_number,
                       argument_count, args, result, errno_value, true);
log_syscall_with_index(0, index, syscall_number,
                       argument_count, args, result, errno_value, false);
set_log_file(0, NULL);
```

The strace 7.1 implementation currently supports 32 output slots, uses the native syscall personality, and fixes the maximum decoded string length at 1000 bytes.

