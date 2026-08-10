CC ?= cc
TEST_CPPFLAGS ?=
TEST_CFLAGS ?= -O2 -Wall -Wextra -Werror
TEST_LDFLAGS ?=
TEST_LDLIBS ?= -pthread -lm

STRACE_DIR := strace
BUILD_DIR := build
STRACE_BUILD_DIR := $(BUILD_DIR)/strace
LIBSCLOG := $(BUILD_DIR)/libsclog.a
TEST_BINARY := $(BUILD_DIR)/sclog_test

STRACE_CONFIGURE_FLAGS ?= --enable-mpers=no --enable-stacktrace=no --without-libselinux

.PHONY: all clean strace-build test

all: $(LIBSCLOG)

$(STRACE_DIR)/configure: $(STRACE_DIR)/configure.ac $(STRACE_DIR)/bootstrap
	cd $(STRACE_DIR) && ./bootstrap

$(STRACE_BUILD_DIR)/Makefile: $(STRACE_DIR)/configure
	mkdir -p $(STRACE_BUILD_DIR)
	cd $(STRACE_BUILD_DIR) && $(abspath $(STRACE_DIR))/configure $(STRACE_CONFIGURE_FLAGS)

strace-build: $(STRACE_BUILD_DIR)/Makefile
	$(MAKE) -C $(STRACE_BUILD_DIR)

$(LIBSCLOG): strace-build
	cp $(STRACE_BUILD_DIR)/src/libsclog.a $@

$(TEST_BINARY): tests/sclog_test.c include/sclog.h $(LIBSCLOG)
	$(CC) $(TEST_CPPFLAGS) $(TEST_CFLAGS) -Iinclude $(TEST_LDFLAGS) -o $@ tests/sclog_test.c $(LIBSCLOG) $(TEST_LDLIBS)

test: $(TEST_BINARY)
	$(TEST_BINARY)

clean:
	$(RM) -r $(BUILD_DIR)
