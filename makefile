CC=gcc  
CFLAGS= -g -c -O0 -Wall
OBJ = server.o trie.o funs.o 
LIBS= -lm
OUT  = jobExecutor

$(OUT): $(OBJ)
	$(CC) -g $(OBJ) -o $@ $(LIBS) 

server.o: server.c
	$(CC) $(CFLAGS) server.c

funs.o: funs.c
	$(CC) $(CFLAGS) funs.c

trie.o: trie.c
	$(CC) $(CFLAGS) trie.c


clean:
	rm -rf $(OBJ) $(OUT) jwpipe* wjpipe* log/Worker_*.txt log/lg.txt
