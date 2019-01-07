CC=g++
CFLAGS=-I.
DEPS = Maze.h
OBJS = Maze.o Main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shortestpathdistancealgos: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
