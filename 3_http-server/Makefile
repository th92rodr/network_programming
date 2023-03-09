
port="8080"

server-build:
	gcc -c ../1_server-client/server_functions.c
	gcc -c http.c
	gcc -c server.c
	gcc server_functions.o http.o server.o -o server.bin

server-debug:
	gcc -c ../1_server-client/server_functions.c
	gcc -c http.c
	gcc -c server.c
	gcc server_functions.o http.o server.o -o server.bin -ggdb
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --track-fds=yes ./server.bin -p $(port)

server-run:
	./server.bin -p $(port)

server-all: server-build server-run

clean:
	@rm -rf *.o
	@rm -rf *.bin
