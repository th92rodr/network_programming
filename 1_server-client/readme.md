Server-Client stream socket implementation.

- Socket stream
- Blocking syscalls

### Packet structure

```
_______ 6 ________ Hello!
(message_length) __ (message)
```

Total packet size: message_length + 1

### Running

```sh
gcc server.c -o server.o
./server.o [-p port]
```

```sh
gcc client.c -o client.o
./client.o [-h host] [-p port]
```
