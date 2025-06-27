# chat

This project is for the coding challenge realtime chat described [here](https://codingchallenges.fyi/challenges/challenge-realtime-chat).

## Build
```
$ clang++ server.cpp -o server
$ clang++ client.cpp -o client
```

## Usage
Run the server
```bash
$ ./server
$$ echoserver $$
Server: listening on the port 7007
```

Run the client
```bash
$ ./client
$$ client for echoserver $$
Client: connected to server!
Type message to send to server. Type 'quit' to stop.
```
