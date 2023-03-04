
server-build:
	gcc -c shared.c
	gcc -c server_functions.c
	gcc -c server.c
	gcc shared.o server_functions.o server.o -o server.bin

server-run:
	./server.bin

server: server-build server-run


client-build:
	gcc -c shared.c
	gcc -c client_functions.c
	gcc -c client.c
	gcc shared.o client_functions.o client.o -o client.bin

client-run:
	./client.bin

client: client-build client-run

clean:
	@rm -rf *.o
	@rm -rf *.bin
