#define _GNU_SOURCE

#include <sclog.h>

#include <sched.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>

struct syscall_case {
	intptr_t number;
	int arg_count;
	intptr_t args[6];
	intptr_t result;
	intptr_t error_number;
};

static struct stat stat_buffer = {
	.st_dev = 123,
	.st_ino = 456,
	.st_uid = 1000,
	.st_gid = 1000,
};

static struct syscall_case cases[] = {
	{
		.number = SYS_clone,
		.arg_count = 4,
		.args = {
			CLONE_NEWIPC | CLONE_NEWUSER | SIGCHLD,
			0x12345678,
			0x7654321,
			0,
		},
		.result = 1234,
	},
	{
		.number = SYS_fstat,
		.arg_count = 2,
		.args = {
			88,
			(intptr_t) &stat_buffer,
		},
		.result = 0,
	},
};

static int
verify_output(FILE *log_file)
{
	char output[4096];

	if (fseek(log_file, 0, SEEK_SET) != 0) {
		perror("fseek");
		return 1;
	}

	size_t bytes_read = fread(output, 1, sizeof(output) - 1, log_file);
	if (ferror(log_file)) {
		perror("fread");
		return 1;
	}
	output[bytes_read] = '\0';

	if (!strstr(output, "0: clone(") || !strstr(output, "1: fstat(")) {
		fputs("unexpected libsclog output:\n", stderr);
		fputs(output, stderr);
		return 1;
	}

	return 0;
}

static int
run_test(void)
{
	FILE *log_file = tmpfile();
	if (!log_file) {
		perror("tmpfile");
		return 1;
	}

	init_sclog();
	set_log_file(0, log_file);

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		log_syscall_with_index(0, (unsigned int) i, cases[i].number,
				       cases[i].arg_count, cases[i].args,
				       cases[i].result, cases[i].error_number,
				       true);
		log_syscall_with_index(0, (unsigned int) i, cases[i].number,
				       cases[i].arg_count, cases[i].args,
				       cases[i].result, cases[i].error_number,
				       false);
	}

	int failed = 0;
	if (get_trace_size(0) <= 0) {
		fputs("libsclog produced no output\n", stderr);
		failed = 1;
	} else {
		failed = verify_output(log_file);
	}

	set_log_file(0, NULL);
	if (fclose(log_file) != 0) {
		perror("fclose");
		failed = 1;
	}

	return failed;
}

int
main(void)
{
	return run_test();
}
