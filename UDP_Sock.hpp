#ifndef UDP_SOCK_HPP
#define UDP_SOCK_HPP 

#include "NetEvent.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAX_MSG 100

class UDP_Sock {
public:
    bool create_server();
    bool create_client(char*);

    void snd(const NetEvent&);
    void rcv(NetEvent&);
    void sndrcv(char*);
    void rcvsnd();
    int isReadable(int sd,int * error);
private:
    int sd, rc, i, n, error;
    char msg[MAX_MSG];
    socklen_t localLen, remoteLen;
    struct sockaddr_in localAddr, remoteAddr;
    struct hostent *h;
	fd_set fds;			/* Set of file descriptors to poll*/
};


#endif /* UDP_SOCK_HPP */
