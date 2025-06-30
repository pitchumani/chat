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

#include "server.h"

bool verbose = true;

Users *Users::instance =  nullptr;
Users *Users::Get() {
	if (!instance) {
        instance = new Users();
	}
	return instance;
}

void Users::addUser(int sock, sockaddr *addr) {
	std::lock_guard<std::mutex> lock(users_mtx);
	users.push_back(new User(sock, addr));
}

int Users::getMaxSocket() {
	std::lock_guard<std::mutex> lock(users_mtx);
	int maxsock = -1;
	for (auto &u : users) {
		if (u->sock > maxsock) {
			maxsock = u->sock;
		}
	}
	return maxsock;
}

int Users::addSocketsToListen(fd_set *fds) {
	int max_sock = -1;
	std::lock_guard<std::mutex> lock(users_mtx);
	if (users.size() == 0) {
		// std::cout << "No users to listen!\n";
		return -1;
	}
	// std::cout << "Added sockets ";
	for (auto &u : users) {
		FD_SET(u->sock, fds);
		if (max_sock < u->sock) max_sock = u->sock;
		// std::cout << u->sock << " ";
	}
	// std::cout << " to listen. max is " << max_sock << std::endl;
	return max_sock;
}

void Users::close(int client_socket) {
	std::lock_guard<std::mutex> lock(users_mtx);
	auto it = std::find_if(users.begin(), users.end(), [client_socket](User *u) {
		return client_socket == u->sock;
	});
	if (it != users.end()) {
		users.erase(it);
	}
}

void Users::closeSockets() {
	std::lock_guard<std::mutex> lock(users_mtx);
	for (const auto &u : users) {
		close(u->sock);
	}
}

int Users::readMessage(fd_set *fds, std::string &msg) {
	std::lock_guard<std::mutex> lock(users_mtx);
	for (auto const &u : users) {
		if (!FD_ISSET(u->sock, fds)) continue;
		int client_socket = u->sock;
		char buffer[256];
		std::memset(buffer, 0, 256);
        int n = read(client_socket, buffer, 256);
        if (n < 0) {
            throw std::runtime_error("Server: read() failed!");
        }
		msg += buffer;
		return client_socket;
	}
	return -1;
}

/// listen for new connections on the server socket
void handleNewConnections(int ssocket) {
	while (true) {
		fd_set read_sockets;
		FD_ZERO(&read_sockets);
		FD_SET(ssocket, &read_sockets);
		int activity = select(ssocket + 1, &read_sockets, NULL, NULL, NULL);
		if (activity < 0) {
			// continue if any interrupt occurred on select()
			if (errno == EINTR) continue;
			throw std::runtime_error("Server: select() failed while waiting for "
									 "new connections on socket " + std::to_string(ssocket));
		}
		// continuing without checking ssocket is set as there is no other socket
		// assert(!FD_ISSET(ssocket, &read_sockets));
		struct sockaddr_in *addr = new sockaddr_in();
		socklen_t socklen;
		int clsocket = accept(ssocket, (struct sockaddr*)&addr, &socklen);
		if (clsocket == -1) {
			std::cerr << "Server: accept() failed! (errno: " << errno << ")" << std::endl;
			break;
		} else if (verbose) {
			std::cout << "Server: connection accepted!" << std::endl;
		}
		Users::Get()->addUser(clsocket, (struct sockaddr*)addr);
		if (verbose) {
			std::cout << "Server: added client with socket "
					  << clsocket << std::endl;
		}
	}
}

void runServer(int port) {
    int server_socket;
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

	// Get users instance
	Users *users = Users::Get();

    // start a thread to accept new connections
	auto conns_thread = std::thread(handleNewConnections, server_socket);

	// timeval to wait (5 secs, 500 milliseconds)
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 500;

    while(true) {
		// socket sets to watch
		fd_set read_fds;
		// clear the set after each activity
		FD_ZERO(&read_fds);
		// add users socket to the listening set
		int max_fd = users->addSocketsToListen(&read_fds);

		// wait for the activity from client sockets (read_fds)
		int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
		if (activity < 0) {
            // continue if there is any interrupt on select()
			if (errno == EINTR) continue;
			throw std::runtime_error("Server: select() failed!");
		}
		std::string msg;
		int client_socket = users->readMessage(&read_fds, msg);
		// FIXME: for now, continue if no valid client socket
		if (client_socket == -1) continue;

        // // print the received message
        std::cout << "recv: " << msg << std::endl;
        // quit the connection if the message is 'quit'
		std::size_t found_colon = msg.find_first_of(":");
		std::string msgstr;
		if (found_colon != std::string::npos) {
			msgstr = msg.substr(found_colon + 1);
		} else {
			msgstr = msg;
		}
        if (msgstr == "quit") {
            std::cout << "Quitting the client!" << std::endl;
			users->close(client_socket);
			// TODO: broadcast to others that user quits
            continue;
        }

		// TODO: send the message to all online users, but sender
        // send the same message received (echo)
        int n = write(client_socket, msg.c_str(), msg.size());
        if (n < 0) {
            throw std::runtime_error("Server: write() failed!");
        }
        if (verbose) {
            std::cout << "Server: sent the response (echo)" << std::endl;
        }
    }

	// close the listening thread
	conns_thread.join();

    // close the connections
    close(server_socket);
    users->closeSockets();
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
