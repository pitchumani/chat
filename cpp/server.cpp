/*
 * https://codingchallenges.fyi/challenges/challenge-realtime-chat
 *
 * TCP Based Echo Service
 * One echo service is defined as a connection based application on TCP. A server
 * listens for TCP connections on TCP port 7. Once a connection is established any
 * data received is sent back. This continues until the calling user terminates
 * the connection.
 *
 * In this example, create simple TCP/IP server that will listen on port 7007
 * (we’re going to use 7007 instead of 7 as ports below 1024 require elevated
 * privileges on many operating systems).
 * When the server receives a message it should echo it back to the client.
 * For now you only need to handle one client connection at a time.
 */

// server

#include <sys/socket.h>   // socket, bind, listen, accept, connect, send, recv
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>
#include <unistd.h>       // close
#include <iostream>
// #include <exception>
#include <stdexcept>       // includes exception. runtime_error etc

bool verbose = true;

void runServer(int port) {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t addrlen = sizeof(client_address);

    // server socket creation
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::runtime_error("Server socket creation failed!");
    }

    // socket configuration
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    // bind the socket
    int res = bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if (res < 0) {
        std::runtime_error("Server: bind() failed!");
    }

    // listen at the address for connect requests (backlog queue 5)
    res = listen(server_socket, 5);
    if (res < 0) {
        std::runtime_error("Server: listen() failed!");
    }

    if (verbose) {
        std::cout << "Server: listening on the port " << port << std::endl;
    }

    // accept a connection
    socklen_t client_socklen;
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_socklen);
    if (client_socket == -1) {
        std::runtime_error("Server: accept() failed!");
    }

    if (verbose) {
        std::cout << "Server: connection accepted!" << std::endl;
    }

    while(true) {
        char buffer[256];
        memset(buffer, 0, 256);
        int n = read(client_socket, buffer, 256);
        if (n < 0) {
            std::runtime_error("Server: read() failed!");
        }
        // print the received message
        std::cout << "recv: " << buffer << std::endl;
        // quit the connection if the message is 'quit'
        if (strcmp(buffer, "quit") == 0) {
            std::cout << "Quitting the server!" << std::endl;
            break;
        }

        // send the same message received (echo)
        std::string res_msg(buffer);
        n = write(client_socket, res_msg.c_str(), res_msg.size());
        if (n < 0) {
            std::runtime_error("Server: write() failed!");
        }
        if (verbose) {
            std::cout << "Server: sent the response (echo)" << std::endl;
        }
    }

    // close the connections
    close(server_socket);
    close(client_socket);
}

int main(int argc, char*argv[]) {
    std::cout << "$$ echoserver $$\n";
    try {
        runServer(7007);
    } catch (std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    return 0;
}
