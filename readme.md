# Client-Server-HashTable

This project implements a client-server architecture where the server manages a hash table, while the client interacts with the server by sending requests via shared memory. The hash table supports concurrent operations (insert, read, delete), using a readers-writer lock for thread safety. Collisions in the hash table are resolved using linked lists.

## Server

The server manages a hash table and processes incoming requests from the clients.

### Build Instructions

To compile the server application, navigate to the `server` directory and execute the Makefile:

```bash
cd server
make
```

### Running the Server

After building the server program, you can run the server by simply running the following command in the `server` directory:

```bash
./server_program
```

It is intended to run only one server instance at a time.

## Client

The client interacts with the server using a menu-driven commandline interface to enqueue operations that the server will process.

### Build Instructions

To compile the client application, navigate to the `client` directory and execute the Makefile:

```bash
cd client
make
```

### Running the Client

Once the client program is built and the server is running, you can run one or more instances of the client concurrently. In the `client` directory simply run:

```bash
./client_program
```
