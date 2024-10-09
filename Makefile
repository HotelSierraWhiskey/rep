##################################################
# Common
##################################################
CC = gcc

DBGFLAGS = 	-DDEBUG_IO -DDEBUG_LEX -DDEBUG_PARSE -DDEBUG_CODE_GEN -DBUILD_DEBUG

CFLAGS = -Wall -Wno-switch -g $(DBGFLAGS)
COMMON_INC = -I.
COMMON_SRCS = io_handler.c lex.c parse.c scratch_register.c code_gen.c

##################################################
# Unity & Test Stuff
##################################################

# All unit test dirs and targets
UNIT_TEST_DIRS = unit_io_handler unit_lex unit_parse
UNIT_TEST_TARGETS = $(UNIT_IO_HANDLER) $(UNIT_LEX) $(UNIT_PARSE)

# Unity flags, includes, srcs
UNITY_FLAGS = -DUNITY_SKIP_DEFAULT_RUNNER -DUNITY_INCLUDE_PRINT_FORMATTED -DUNITY_OUTPUT_COLOR

UNITY_INC = $(COMMON_INC) \
			-Itests/unity/src \
			-Itests/unity/extras/fixture/src \
			-Itests/unity/extras/memory/src \

UNITY_SRCS = 	tests/unity/src/unity.c \
				tests/unity/extras/fixture/src/unity_fixture.c \
				tests/unity/extras/memory/src/unity_memory.c \

# All testing-related flags, includes, srcs
TEST_FLAGS = $(UNITY_FLAGS)

TEST_INC = $(UNITY_INC)

TEST_SRCS = $(UNITY_SRCS)

# Rule to build test srcs
%._test.o: %.c
	$(CC) $(CFLAGS) $(TEST_FLAGS) $(TEST_INC) -c $< -o $@

##################################################
# Unit IO Handler
##################################################
UNIT_IO_HANDLER = unit_io_handler
UNIT_IO_HANDLER_PATH = tests/$(UNIT_IO_HANDLER)
UNIT_IO_HANDLER_TARGET = $(UNIT_IO_HANDLER_PATH)/$(UNIT_IO_HANDLER)
UNIT_IO_HANDLER_SRCS = $(COMMON_SRCS) $(TEST_SRCS) $(UNIT_IO_HANDLER_PATH)/$(UNIT_IO_HANDLER).c
UNIT_IO_HANDLER_OBJS = $(COMMON_SRCS:.c=.o) $(TEST_SRCS:.c=._test.o) $(UNIT_IO_HANDLER_PATH)/$(UNIT_IO_HANDLER)._$(UNIT_IO_HANDLER).o

%._$(UNIT_IO_HANDLER).o: %.c
	$(CC) $(CFLAGS) $(TEST_FLAGS) $(TEST_INC) -c $< -o $@

$(UNIT_IO_HANDLER_TARGET): $(UNIT_IO_HANDLER_OBJS)
	$(CC) $(UNIT_IO_HANDLER_OBJS) -o $(UNIT_IO_HANDLER_TARGET)

$(UNIT_IO_HANDLER): $(UNIT_IO_HANDLER_TARGET)

##################################################
# Unit Lex
##################################################
UNIT_LEX = unit_lex
UNIT_LEX_PATH = tests/$(UNIT_LEX)
UNIT_LEX_TARGET = $(UNIT_LEX_PATH)/$(UNIT_LEX)
UNIT_LEX_SRCS = $(COMMON_SRCS) $(TEST_SRCS) $(UNIT_LEX_PATH)/$(UNIT_LEX).c
UNIT_LEX_OBJS = $(COMMON_SRCS:.c=.o) $(TEST_SRCS:.c=._test.o) $(UNIT_LEX_PATH)/$(UNIT_LEX)._$(UNIT_LEX).o

%._$(UNIT_LEX).o: %.c
	$(CC) $(CFLAGS) $(TEST_FLAGS) $(TEST_INC) -c $< -o $@

$(UNIT_LEX_TARGET): $(UNIT_LEX_OBJS)
	$(CC) $(UNIT_LEX_OBJS) -o $(UNIT_LEX_TARGET)

$(UNIT_LEX): $(UNIT_LEX_TARGET)

##################################################
# Unit Parse
##################################################
UNIT_PARSE = unit_parse
UNIT_PARSE_PATH = tests/$(UNIT_PARSE)
UNIT_PARSE_TARGET = $(UNIT_PARSE_PATH)/$(UNIT_PARSE)
UNIT_PARSE_SRCS = $(COMMON_SRCS) $(TEST_SRCS) $(UNIT_PARSE_PATH)/$(UNIT_PARSE).c
UNIT_PARSE_OBJS = $(COMMON_SRCS:.c=.o) $(TEST_SRCS:.c=._test.o) $(UNIT_PARSE_PATH)/$(UNIT_PARSE)._$(UNIT_PARSE).o

%._$(UNIT_PARSE).o: %.c
	$(CC) $(CFLAGS) $(TEST_FLAGS) $(TEST_INC) -c $< -o $@

$(UNIT_PARSE_TARGET): $(UNIT_PARSE_OBJS)
	$(CC) $(UNIT_PARSE_OBJS) -o $(UNIT_PARSE_TARGET)

$(UNIT_PARSE): $(UNIT_PARSE_TARGET)

##################################################
# Main Application
##################################################
TARGET = rep
SRCS = $(COMMON_SRCS) main.c
OBJS = $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(INC) $(OBJS) -o $(TARGET)

compile: $(TARGET)

##################################################
# Utils
##################################################
clean:
	rm -f $(TARGET) $(OBJS) $(UNIT_IO_HANDLER_TARGET) $(UNIT_IO_HANDLER_OBJS) $(UNIT_LEX_TARGET) $(UNIT_LEX_OBJS) $(UNIT_PARSE_TARGET) $(UNIT_PARSE_OBJS)

run:
	./rep

# Build and run all unit tests
test: $(UNIT_TEST_TARGETS)
	@for dir in $(UNIT_TEST_DIRS); do \
		echo "\033[1m\033[34m\nRunning tests in $$dir\n\033[0m"; \
		(cd tests/$$dir && ./$$dir); \
	done

.PHONY: compile unit_io_handler unit_lex unit_parse clean run