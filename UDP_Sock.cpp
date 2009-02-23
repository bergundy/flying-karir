#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */
#include "UDP_Sock.hpp"
#include "NetEvent.hpp"
#include "Ship.hpp"

#define PORT 1500
#define MAX_MSG 100
#define SOCKET_ERROR -1
#define FLAGS 0
#define TIMEOUT 0 // ms

int UDP_Sock::isReadable(int sd,int * error) { // milliseconds
  fd_set socketReadSet;
  FD_ZERO(&socketReadSet);
  FD_SET(sd,&socketReadSet);
  struct timeval tv;
  if (TIMEOUT) {
    tv.tv_sec  = TIMEOUT / 1000;
    tv.tv_usec = (TIMEOUT % 1000) * 1000;
  } else {
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
  } // if
  if (select(sd+1,&socketReadSet,0,0,&tv) == SOCKET_ERROR) {
    *error = 1;
    return 0;
  } // if
  *error = 0;
  return FD_ISSET(sd,&socketReadSet) != 0;
} /* isReadable */

//template <class Serial>
    //void UDP_Sock::snd(const Serial& serial)
    void UDP_Sock::snd(const NetEvent& serial)
{

    std::ostringstream os;
    boost::archive::text_oarchive oa(os);
    oa << serial;
    std::string message = os.str();
    
    rc = sendto(sd, message.c_str(), message.size(), FLAGS, 
		(struct sockaddr *) &remoteAddr, sizeof(remoteAddr));

    if(rc<0) {
        std::cout << "cannot send data " << std::endl;
        close(sd);
        return;
    }
}
//template <class Serial>
    //bool UDP_Sock::rcv(Serial& serial)
    bool UDP_Sock::rcv(NetEvent& serial)
{
    memset(msg,0x0,MAX_MSG);
    remoteLen = sizeof(remoteAddr);

    if (!isReadable(sd,&error)) return false;

    /* receive echoed message */
    n = recvfrom(sd, msg, MAX_MSG, FLAGS,
      (struct sockaddr *) &remoteAddr, &remoteLen);

    if(n<0) {
        std::cerr << "cannot receive data\n";
        return false;
    }

    /* print received message */
    std::cout << "echo from " << inet_ntoa(remoteAddr.sin_addr)
              << " UDP:" << ntohs(remoteAddr.sin_port)
              << ": " << msg << std::endl;
    
    std::istringstream is(std::string(msg, strlen(msg)));
    boost::archive::text_iarchive ia(is);
    ia >> serial;
    return true;
}

bool UDP_Sock::create_server() {
    /* socket creation */
    sd=socket(AF_INET, SOCK_DGRAM, 0);
    if(sd<0) {
        std::cerr << "cannot open socket \n";
        return false;
    }

    /* bind local server port */
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(PORT);
    rc = bind (sd, (struct sockaddr *) &localAddr,sizeof(localAddr));
    if(rc<0) {
        std::cerr << "cannot bind port number " << PORT << std::endl;
        return false;
    }
    std::cout << "waiting for data on port udp " << PORT << std::endl;

    return true;
}

bool UDP_Sock::create_client(char* host) {
    h = gethostbyname(host);
    if(h==NULL) {
        std::cerr << "unknown host: " << host << std::endl;
        return false;
    }

    std::cout << "starting to send data to " << h->h_name << " IP: " << 
                 inet_ntoa(*(struct in_addr *)h->h_addr_list[0]) << std::endl;

    remoteAddr.sin_family = h->h_addrtype;
    memcpy((char *) &remoteAddr.sin_addr.s_addr, 
        h->h_addr_list[0], h->h_length);
    remoteAddr.sin_port = htons(PORT);

    /* socket creation */
    sd = socket(AF_INET,SOCK_DGRAM,0);
    if(sd<0) {
        std::cerr << "cannot open socket\n";
        return false;
    }

    /* bind any port */
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(0);

    rc = bind(sd, (struct sockaddr *) &localAddr, sizeof(localAddr));
    if(rc<0) {
        std::cerr << "cannot bind port\n";
        return false;
    }
    
    return true;
}
