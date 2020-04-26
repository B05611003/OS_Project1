CC = gcc
CFLAGS = -Wall

# Should be equivalent to your list of C files, if you don't build selectively
PROJECT = ./main
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)


$(PROJECT): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@