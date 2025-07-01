#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>   // socket, bind, listen, accept, connect, send, recv
#include <sys/time.h>     // struct timeval
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>
#include <unistd.h>       // close

#include <algorithm>      // std::find
#include <cstring>        // memset
#include <iostream>
#include <mutex>
#include <stdexcept>      // includes exception. runtime_error etc
#include <string>
#include <thread>
#include <vector>

#include "user.h"

class Users {
	Users(){}
	static Users *instance;
	std::vector<User*> users;
	std::mutex users_mtx;

public:
	static Users *Get();
	void addUser(int sock, sockaddr *addr);
	int getMaxSocket();
	// add client sockets to listen list, return max socket id
	int addSocketsToListen(fd_set *fds);
	int readMessage(fd_set *fds, std::string &msg);
	bool broadcastMessage(int sender, const std::string &msg);
	void close(int sock);
	void closeSockets();
};

#endif // SERVER_H
