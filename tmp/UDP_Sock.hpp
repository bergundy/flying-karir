#ifndef UDP_SOCK_HPP
#define UDP_SOCK_HPP 

#include "NetEvent.hpp"
#include "Ship.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAX_MSG 100

class Sock {
public:
    bool create_server();
    bool create_client(char*);

    //template <class Serial> void snd(const Serial&);
    //template <class Serial> bool rcv(Serial&);
    void snd(const NetEvent&);
    bool rcv(NetEvent&);
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
