server: server.o commons.o
	gcc -Wall -g server.o commons.o -o server

client: client.o commons.o
	gcc -Wall -g client.o commons.o -o client

server.o: server.c commons.c
	gcc -Wall -c -g server.c commons.c

commons.o: commons.c commons.h
	gcc -Wall -c -g commons.c

clean:
	@rm -f *.o server client