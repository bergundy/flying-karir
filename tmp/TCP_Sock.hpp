#ifndef TCP_SOCK_HPP
#define TCP_SOCK_HPP 

#include "NetEvent.hpp"
#include "Ship.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAXMSG 1024

class Sock {
public:
    bool create_server();
    bool create_client(std::string);

    //template <class Serial> void snd(const Serial&);
    //template <class Serial> bool rcv(Serial&);
    void snd(const NetEvent&);
    //void snd(const NetEvent&,std::string);
    bool rcv(NetEvent&,std::string);
    //int isReadable(int sd,int * error);
private:
    socklen_t remoteLen;
    struct sockaddr_in localAddr, remoteAddr;
    struct hostent *server;
    
    fd_set master;     // master file descriptor list
    fd_set read_fds;   // temp file descriptor list for select()
    int listener;      // listening socket descriptor
    int newfd;         // newly accept()ed socket descriptor
    int fdmax;         // maximum file descriptor number

    char buf[MAXMSG]; // buffer for client data
    int nbytes;
    int yes, i, j;

};

#endif /* TCP_SOCK_HPP */
