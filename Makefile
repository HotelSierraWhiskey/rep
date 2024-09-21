CC = gcc


DBGFLAGS = 	-DDEBUG_IO\
			-DDEBUG_LEX\
			-DDEBUG_PARSE\

CFLAGS = -Wall -Wno-switch -g $(DBGFLAGS)

TARGET = rep

SRCS = io_handler.c lex.c parse.c main.c
OBJS = $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

compile: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

run:
	./rep

.PHONY: compile clean run