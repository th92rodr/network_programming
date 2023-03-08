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
make server-build # for building

./server.bin [-p port] # or
make server-run port="8080" # for running
```

```sh
make client-build # for building

./client.bin [-h host] [-p port] # or
make client-run host="localhost" port="8080" # for running
```
