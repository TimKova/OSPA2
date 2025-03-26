CC = gcc
CFLAGS = -Wall -pthread

OBJS = chash.o commands.o threads.o hash_table.o

chash: $(OBJS)
	$(CC) $(CFLAGS) -o chash $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	del /f *.o chash.exe output.txt
