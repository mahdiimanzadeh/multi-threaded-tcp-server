Here is an example content for your `README.md` file, including descriptions of both C files (`server.c` and `client.c`) and the expected output.

### README.md

```markdown
# Multi-threaded TCP Server

This project implements a multi-threaded TCP server in C that supports concurrent file read and write operations from multiple clients. The server uses a thread pool to handle multiple client connections efficiently and synchronizes file operations to maintain data integrity.

## Objectives

- Develop a TCP server that listens for and handles multiple client connections.
- Implement a thread pool to manage concurrent client connections.
- Provide file read and write capabilities for clients.
- Ensure proper synchronization to avoid race conditions and maintain data consistency.

## Implementation Details

### Server (`server.c`)

The server is designed to handle multiple client connections concurrently using a thread pool. It supports two primary operations requested by clients: writing to a file and reading from a file.

- **Thread Pool Initialization:** A fixed-size thread pool is created at the start of the server. Each thread waits for client connections and handles their requests.
- **Client Connection Handling:** The server listens for incoming client connections and assigns them to worker threads from the pool.
- **File Operations:** The server can handle `WRITE:<message>` commands to append messages to a file and `READ` commands to read the contents of the file.
- **Synchronization:** Mutexes are used to synchronize file operations, ensuring that only one thread accesses the file at a time to prevent race conditions and data corruption.

### Client (`client.c`)

The client continuously prompts the user for commands and sends them to the server. It supports two commands: writing a message to the file on the server and reading the contents of the file.

- **Continuous Operation:** The client runs in a loop, allowing the user to send multiple commands without exiting.
- **Server Communication:** For each command, the client establishes a new connection to the server, sends the command, and reads the response.
- **Commands:**
  - `WRITE:<message>`: Sends a request to write the specified message to the file on the server.
  - `READ`: Sends a request to read the contents of the file from the server.
```


Server Output

When running the server, the output will display messages from the clients, indicating the operations being performed.
```

Server listening on port 8080
Message from client: WRITE:Hello from Client 1
Message from client: READ
Message from client: WRITE:This is Client 2
Message from client: READ
```

#Client Output

**Client 1:**

```
Enter command (WRITE:<message> or READ): WRITE:Hello from Client 1
Response from server: Write successful
Enter command (WRITE:<message> or READ): READ
Response from server: Hello from Client 1
```

**Client 2:**

```
Enter command (WRITE:<message> or READ): WRITE:This is Client 2
Response from server: Write successful
Enter command (WRITE:<message> or READ): READ
Response from server: Hello from Client 1
This is Client 2
```

## How to Run

### Compile the Server and Client

```sh
gcc -pthread server.c -o server
gcc client.c -o client
```

### Run the Server

Start the server by running:

```sh
./server
```

### Run the Client

In a separate terminal, run the client:

```sh
./client
```

### Commands

- **WRITE:** Enter `WRITE:<message>` to write a message to the server file.
- **READ:** Enter `READ` to read the contents of the server file.
