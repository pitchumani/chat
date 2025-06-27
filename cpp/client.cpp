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

// client

#include <sys/socket.h>   // socket, bind, listen, accept, connect, send, recv
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>
#include <unistd.h>       // close
#include <iostream>
// #include <exception>
#include <stdexcept>       // includes exception. runtime_error etc

bool verbose = true;

void runClient(int port) {
    // client socket creation
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::runtime_error("Server socket creation failed!");
    }

    // socket configuration
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");  // connect to localhost
    server_address.sin_port = htons(port);

    // connect to server
    int res = connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if (res < 0) {
        std::runtime_error("Client: connect() failed!");
    }
    if (verbose) {
        std::cout << "Client: connected to server!" << std::endl;
    }

    std::cout << "Type message to send to server. Type 'quit' to stop." << std::endl;
    while(true) {
        std::string msg;
        std::getline(std::cin, msg);
        send(client_socket, msg.c_str(), msg.size(), 0);  // no flags for now
        if (verbose) {
            std::cout << "Client:: sent message '" << msg << "'\n";
        }

        // terminate if user enters 'quit'
        if (msg == std::string("quit")) {
            break;
        }

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer) - 1, 0);  // no flags for now
        std::cout << "recv: " << buffer << std::endl;
    }

    // close the connections
    close(client_socket);
}

int main(int argc, char*argv[]) {
    std::cout << "$$ client for echoserver $$\n";
    try {
        runClient(7007);
    } catch (std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    return 0;
}
