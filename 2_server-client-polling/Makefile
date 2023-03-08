
port="8080"

server-build:
	gcc -c ../1_server-client/shared.c
	gcc -c ../1_server-client/server_functions.c
	gcc -c poll.c
	gcc -c server.c
	gcc shared.o server_functions.o poll.o server.o -o server.bin

server-debug:
	gcc -c ../1_server-client/shared.c
	gcc -c ../1_server-client/server_functions.c
	gcc -c poll.c
	gcc -c server.c
	gcc shared.o server_functions.o poll.o server.o -o server.bin -ggdb
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --track-fds=yes ./server.bin -p $(port)

server-run:
	./server.bin -p $(port)

server-all: server-build server-run

clean:
	@rm -rf *.o
	@rm -rf *.bin
