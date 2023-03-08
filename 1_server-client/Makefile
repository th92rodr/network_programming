
host="localhost"
port="8080"

server-build:
	gcc -c shared.c
	gcc -c server_functions.c
	gcc -c server.c
	gcc shared.o server_functions.o server.o -o server.bin

server-debug:
	gcc -c shared.c
	gcc -c server_functions.c
	gcc -c server.c
	gcc shared.o server_functions.o server.o -o server.bin -ggdb
	valgrind --leak-check=yes --leak-resolution=high --track-origins=yes --track-fds=yes ./server.bin -p $(port)

server-run:
	./server.bin -p $(port)

server-all: server-build server-run


client-build:
	gcc -c shared.c
	gcc -c client_functions.c
	gcc -c client.c
	gcc shared.o client_functions.o client.o -o client.bin

client-debug:
	gcc -c shared.c
	gcc -c client_functions.c
	gcc -c client.c
	gcc shared.o client_functions.o client.o -o client.bin -ggdb
	valgrind --leak-check=yes --leak-resolution=high --track-origins=yes --track-fds=yes ./client.bin -h $(host) -p $(port)

client-run:
	./client.bin -h $(host) -p $(port)

client-all: client-build client-run


clean:
	@rm -rf *.o
	@rm -rf *.bin
