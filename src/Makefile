CC = gcc
MAIN = test.c
BINARY = test
SOURCES = stack/*.c queue/*.c
INCLUDES = -I stack -I queue

compile:
	@$(CC) $(MAIN) $(SOURCES) -o $(BINARY) $(INCLUDES) -Wall -Werror

run:
	@./$(BINARY)

valgrind:
	@valgrind ./$(BINARY)
