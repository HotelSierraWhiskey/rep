##################################################
# Common
##################################################
CC = gcc

DBGFLAGS = 	-DDEBUG_IO\
			-DDEBUG_LEX\
			-DDEBUG_PARSE\
			-DBUILD_DEBUG

CFLAGS = -Wall -Wno-switch -g $(DBGFLAGS)
COMMON_SRCS = io_handler.c lex.c parse.c

##################################################
# Unity Stuff
##################################################
UNITY_SRCS = tests/unity/src/unity.c
UNITY_INC = -Itests/unity/src

##################################################
# Unit Lex
##################################################
UNIT_LEX_PATH = tests/unit_lex
UNIT_LEX_TARGET = $(UNIT_LEX_PATH)/unit_lex
UNIT_LEX_SRCS = $(COMMON_SRCS) $(UNITY_SRCS) $(UNIT_LEX_PATH)/unit_lex.c
UNIT_LEX_OBJS = $(COMMON_SRCS:.c=.unit.o) $(UNITY_SRCS:.c=.unit.o) $(UNIT_LEX_PATH)/unit_lex.unit.o

%.unit.o: %.c
	$(CC) $(CFLAGS) $(UNITY_INC) -c $< -o $@

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
	rm -f $(TARGET) $(OBJS) $(UNIT_LEX_TARGET) $(UNIT_LEX_OBJS)

run:
	./rep

.PHONY: compile unit_lex clean run