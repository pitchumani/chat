#ifndef USER_H
#define USER_H

class User {
public:
	std::string name;
	int sock;
	sockaddr *address;
    User(int sock, sockaddr *addr) : sock(sock), address(addr) {}
	User *operator=(User* u) = delete;
};

#endif  // USER_H
