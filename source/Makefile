CC = gcc
MAIN = test.c
BINARY = test
SOURCES = stack/*.c queue/*.c deque/*.c vector/*.c
INCLUDES = -I stack -I queue -I deque -I vector
LIBRARIES = -lm

compile:
	@$(CC) $(MAIN) $(SOURCES) -o $(BINARY) $(INCLUDES) $(LIBRARIES) -Wall -Werror -g

run:
	@./$(BINARY)

valgrind:
	@valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./$(BINARY)
