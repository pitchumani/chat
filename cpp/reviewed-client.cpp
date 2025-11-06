#include <sys/socket.h>   // socket, bind, listen, accept, connect, send, recv
#include <sys/select.h>   // select(), fd_set
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>
#include <unistd.h>       // close, STDIN_FILENO
#include <iostream>
#include <stdexcept>       // includes exception. runtime_error etc
#include <string.h>       // For memset

bool verbose = true;

// runClient
// arguments: username, port number
void runClient(const std::string &uname, int port) {
    // client socket creation
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        throw std::runtime_error("Server socket creation failed!"); // Use throw for exceptions
    }

    // socket configuration
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");  // connect to localhost
    server_address.sin_port = htons(port);

    // connect to server
    int res = connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if (res < 0) {
        close(client_socket); // Close the socket on connection failure
        throw std::runtime_error("Client: connect() failed!");
    }
    if (verbose) {
        std::cout << "Client: connected to server!" << std::endl;
    }

    // set client socket fd as max fd
    int max_fd = client_socket;
    // update max_fd if STDIN_FILENO is of greater value
    max_fd = STDIN_FILENO > max_fd ? STDIN_FILENO : max_fd;
    if (verbose) std::cout << "Client: max_fd is set to " << max_fd << std::endl;

    while(true) {
        // Need to wait for user input from stdin and server message
        fd_set read_fds;
        FD_ZERO(&read_fds);  // **clear the set before each select call**
        // add socket that connects to server
        FD_SET(client_socket, &read_fds);
        if (verbose) std::cout << "Client: listening socket " << client_socket << std::endl;

        // add stdin fd also to the file descriptors to watch
        FD_SET(STDIN_FILENO, &read_fds);
        if (verbose) std::cout << "Client: standard input socket " << STDIN_FILENO << std::endl;


        // emit the username: as prompt for user input
        std::cout << uname << ": ";
        std::cout.flush();

        // wait for the activity from the read_fds
        // readlist - read_fds, writelist - NULL, exceptlist - NULL
        // timeval - NULL - to wait indefinitely for activity
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            // Check for EINTR, which can happen if a signal interrupts select()
            if (errno == EINTR) {
                continue;
            }
            close(client_socket);
            throw std::runtime_error("Client: select() failed!");
        }

        // process the user input if found
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            std::string msg;
            std::getline(std::cin, msg);
            std::string prefixed_msg = uname + ":" + msg; // Simpler concatenation
            // no flags for now
            if (send(client_socket, prefixed_msg.c_str(), prefixed_msg.size(), 0) == -1) {
                // Handle send error, e.g., server disconnected
                std::cerr << "Client: send() failed, server might have disconnected." << std::endl;
                break;
            }
            if (verbose) {
                std::cout << "Client:: sent message '" << msg << "'\n";
            }

            // terminate if user enters 'quit' or 'exit'
            if ((msg == std::string("quit")) || (msg == std::string("exit"))) {
                break;
            }
        }

        // receive message from server if any
        if (FD_ISSET(client_socket, &read_fds)) {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0); // no flags for now
            if (bytes_received > 0) {
                std::cout << "recv: " << buffer << std::endl;
            } else if (bytes_received == 0) {
                // Server closed the connection
                std::cout << "Server disconnected." << std::endl;
                break;
            } else {
                // Handle recv error
                std::cerr << "Client: recv() failed." << std::endl;
                break;
            }
        }
    }

    // close the connections
    close(client_socket);
}

int main(int argc, char*argv[]) {
    std::cout << "### client for echoserver ###\n";
    std::cout << "# Enter your username when prompted." << std::endl;
    std::cout << "# Type 'exit' to exit the chat." << std::endl;
    std::string uname;
    std::cout << "Enter your username: ";
    std::getline(std::cin, uname);
    if (uname.empty()) {
        std::cerr << "Invalid username. Aborting." << std::endl;
        return 1;
    }

    try {
        runClient(uname, 7007);
    } catch (const std::exception &e) { // Catch by const reference
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    return 0;
}