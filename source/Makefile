CC = gcc
MAIN = test.c
BINARY = test
SOURCES = stack/*.c queue/*.c deque/*.c
INCLUDES = -I stack -I queue -I deque

compile:
	@$(CC) $(MAIN) $(SOURCES) -o $(BINARY) $(INCLUDES) -Wall -Werror

run:
	@./$(BINARY)

valgrind:
	@valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./$(BINARY)
