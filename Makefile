##################################################
# Common
##################################################
CC = gcc

DBGFLAGS = 	-DDEBUG_IO -DDEBUG_LEX -DDEBUG_PARSE -DBUILD_DEBUG

CFLAGS = -Wall -Wno-switch -g $(DBGFLAGS)
COMMON_INC = -I.
COMMON_SRCS = io_handler.c lex.c parse.c

##################################################
# Unity Stuff
##################################################
UNITY_SRCS = 	tests/unity/src/unity.c \
				tests/unity/extras/fixture/src/unity_fixture.c \
				tests/unity/extras/memory/src/unity_memory.c \

UNITY_INC = $(COMMON_INC) \
			-Itests/unity/src \
			-Itests/unity/extras/fixture/src \
			-Itests/unity/extras/memory/src \

##################################################
# Unit IO Handler
##################################################
UNIT_IO_HANDLER_PATH = tests/unit_io_handler
UNIT_IO_HANDLER_TARGET = $(UNIT_IO_HANDLER_PATH)/unit_io_handler
UNIT_IO_HANDLER_SRCS = $(COMMON_SRCS) $(UNITY_SRCS) $(UNIT_IO_HANDLER_PATH)/unit_io_handler.c
UNIT_IO_HANDLER_OBJS = $(COMMON_SRCS:.c=.unit.o) $(UNITY_SRCS:.c=.unit.o) $(UNIT_IO_HANDLER_PATH)/unit_io_handler.unit.o

%.unit.o: %.c
	$(CC) $(CFLAGS) -DUNITY_SKIP_DEFAULT_RUNNER $(UNITY_INC) -c $< -o $@

$(UNIT_IO_HANDLER_TARGET): $(UNIT_IO_HANDLER_OBJS)
	$(CC) $(UNIT_IO_HANDLER_OBJS) -o $(UNIT_IO_HANDLER_TARGET)

unit_io_handler: $(UNIT_IO_HANDLER_TARGET)

##################################################
# Unit Lex
##################################################
UNIT_LEX_PATH = tests/unit_lex
UNIT_LEX_TARGET = $(UNIT_LEX_PATH)/unit_lex
UNIT_LEX_SRCS = $(COMMON_SRCS) $(UNITY_SRCS) $(UNIT_LEX_PATH)/unit_lex.c
UNIT_LEX_OBJS = $(COMMON_SRCS:.c=.unit.o) $(UNITY_SRCS:.c=.unit.o) $(UNIT_LEX_PATH)/unit_lex.unit.o

%.unit.o: %.c
	$(CC) $(CFLAGS) -DUNITY_SKIP_DEFAULT_RUNNER $(UNITY_INC) -c $< -o $@

$(UNIT_LEX_TARGET): $(UNIT_LEX_OBJS)
	$(CC) $(UNIT_LEX_OBJS) -o $(UNIT_LEX_TARGET)

unit_lex: $(UNIT_LEX_TARGET)

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
	rm -f $(TARGET) $(OBJS) $(UNIT_IO_HANDLER_TARGET) $(UNIT_IO_HANDLER_OBJS) $(UNIT_LEX_TARGET) $(UNIT_LEX_OBJS)

run:
	./rep

.PHONY: compile unit_io_handler unit_lex clean run