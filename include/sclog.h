#ifndef LIBSCLOG_SCLOG_H
#define LIBSCLOG_SCLOG_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void init_sclog(void);
void set_log_file(int log_idx, FILE *file);
void log_syscall_printf(int log_idx, const char *format, ...);
void log_syscall_with_index(int log_idx, unsigned int index, intptr_t scno,
			    int arg_count, intptr_t args[], intptr_t result,
			    intptr_t error_number, bool entering);
int get_trace_size(int log_idx);

#ifdef __cplusplus
}
#endif

#endif /* LIBSCLOG_SCLOG_H */
