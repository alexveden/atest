
BUILD_DIR := build
INC_FLAGS := 
CFLAGS := $(INC_FLAGS) -MMD -MP -Og -g -ggdb -Wall -Wno-unused-function

.PHONY: all clean tests

SRC = $(wildcard test_*.c)
TAR = $(SRC:.c=.test)
TARGETS := $(addprefix $(BUILD_DIR)/,$(TAR))
	
# This must be the first target
all: $(TARGETS)

# EXPLICIT test building! Can be handled for special test cases
# IMPORTANT: --wrapping system functions may lead to unpredictable consequencies
# IMPORTANT: to make testlib.h functions wrappable we need to include .h, and add all dependent .c sources below
$(BUILD_DIR)/test_testlib_syscallmock.test: test_testlib_syscallmock.c ../src/testlib.c ../src/testlib2.c
	@echo "\n\nBuilding custom test: $@ incl ($^)"
	./atest $<
	$(CC) $(CFLAGS) $^ -Wl,--wrap=putc,--wrap=puts,--wrap=add,--wrap=add2 -o $@

#
# # EXPLICIT test building! No need to include whole file, because  
# $(BUILD_DIR)/test_testlib2.test: test_testlib2.c # NO need to include -> ../src/testlib2.c, because #include "..testlib2.c" used 
# 	$(CC) $(CFLAGS) $^ -o $@

# DEFAULT test building (no dependencies!)
$(BUILD_DIR)/%.test: %.c
	@echo "\n\nBuilding generic tests for: $<"
	mkdir -p $(dir $@)
	./atest $<
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir $@

clean:
	rm -r $(BUILD_DIR)

tests: $(TARGETS)
	@echo "------------------------------"
	@echo "Running tests"
	@echo "------------------------------"
	for test in $(BUILD_DIR)/*.test; do ./$$test q; done 
