# C Socket Example: TCP Server and Client

A beginner-friendly TCP client/server pair written in C. The client connects to the server, sends a message, and the server replies.

## Files

| File | Description |
|---|---|
| `server.c` | Listens on port 8080, receives a message, sends a reply |
| `client.c` | Connects to the server, sends a message, prints the reply |

## How the communication works

The server and client never call each other's code. They talk through a TCP connection managed by the operating system.

- The server holds one end of the connection: `client_fd`
- The client holds the other end: `sock`
- Whatever one side `send()`s, the other side `recv()`s

```
 server.c                                           client.c
 --------                                           --------
 client_fd  <========= TCP connection =========>    sock

 send(client_fd, ...)  ---- bytes --->  recv(sock, ...)
 recv(client_fd, ...)  <--- bytes ----  send(sock, ...)
```

## Timeline

```
Time   Server                          Client
----   ------                          ------
 1     accept() waiting...
 2                                     connect()  --> connected!
 3     accept() returns client_fd      send("i love you kue")
 4     recv() gets the message         recv() waiting...
 5     send("Hello from server!")
 6     close()                         recv() gets the reply
 7                                     close()
```

### Step by step

1. **Server waits.** `accept()` blocks until a client connects.
2. **Client connects.** `connect()` dials the server at `127.0.0.1:8080`.
3. **Connection established.** `accept()` returns a new socket, `client_fd`, for this client. The client sends its message with `send()`.
4. **Server receives.** `recv()` copies the message into `buffer`. Meanwhile, the client waits in its own `recv()`.
5. **Server replies.** `send(client_fd, ...)` pushes the reply into the connection.
6. **Server closes** its sockets. The client's `recv()` receives the reply.
7. **Client closes** its socket.

## Key rule

`send` on one side pairs with `recv` on the other side.

| Who sends | Who receives |
|---|---|
| client `send(sock, ...)` | server `recv(client_fd, ...)` |
| server `send(client_fd, ...)` | client `recv(sock, ...)` |

If both sides call `recv()` and nobody calls `send()`, both programs freeze, each waiting for the other.

## `server_fd` vs `client_fd`

| | `server_fd` | `client_fd` |
|---|---|---|
| Created by | `socket()` | `accept()` |
| Purpose | Listening socket (the door) | Connection to one specific client |
| Used with | `bind`, `listen`, `accept` | `send`, `recv` |

## Build

```bash
gcc server.c -o server
gcc client.c -o client
```

## Run

Start the server first, in one terminal:

```bash
./server
```

Then run the client in a second terminal:

```bash
./client
```

## Expected output

**Server terminal**

```
waiting client
client connected
client says: i love you kue
```

**Client terminal**

```
server says: Hello from server!
```

## Troubleshooting

| Problem | Cause |
|---|---|
| `connect failed: Connection refused` | Server is not running, or the ports do not match (both must be `8080`) |
| `bind failed: Address already in use` | An old server is still running, or the port is still reserved. `SO_REUSEADDR` helps with this |
| `undefined reference to 'main'` | The `.c` file is empty or unsaved. Save it and recompile |
| Old behavior after editing code | Recompile with `gcc` after every change |

## Next steps

- Let the client send whatever you type using `fgets()`
- Handle multiple clients with a loop
- Try UDP sockets (`SOCK_DGRAM`)
