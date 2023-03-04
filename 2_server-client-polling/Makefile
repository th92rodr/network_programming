
server-build:
	gcc -c ../1_server-client/shared.c
	gcc -c ../1_server-client/server_functions.c
	gcc -c poll.c
	gcc -c server.c
	gcc shared.o server_functions.o poll.o server.o -o server.bin

server-run:
	./server.bin

server: server-build server-run

clean:
	@rm -rf *.o
	@rm -rf *.bin
