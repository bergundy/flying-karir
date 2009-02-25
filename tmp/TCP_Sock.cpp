#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <iostream>
#include <sstream>
#include <string>
#include "NetEvent.hpp"
#include "TCP_Sock.hpp"

//#include <SDL.h>

#define PORT   2020
#define MAXMSG 1024
#define SLOTS  10
#define FLAGS  0

bool Sock::create_client(std::string servName)
{
    /* clear the master and temp sets */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
 
    /* get the listener */
    if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << "cannot open socket\n";
        return false;
    }
     
    server = gethostbyname(servName.c_str());
    if (server == NULL) {
        std::cerr << "unknown host: " << servName << std::endl;
        return false;
    }
    
    remoteAddr.sin_family = server->h_addrtype;
    memcpy((char *) &remoteAddr.sin_addr.s_addr, 
        server->h_addr_list[0], server->h_length);
    remoteAddr.sin_port = htons(PORT);

    if (connect(listener,(struct sockaddr *) &remoteAddr,sizeof(remoteAddr)) < 0) {
        std::cerr << "unable to connect" << std::endl;
        return false;
    }
/*
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(0);


    if(bind(listener, (struct sockaddr *)&localAddr, sizeof(localAddr)) == -1)
    {
        std::cerr << "cannot bind port number " << PORT << std::endl;
        return false;
    }
    
 
    //--- make into listener with SLOTS slots ---
    if(listen(listener, SLOTS) == -1)
    {
        std::cerr << "cannot listen to port number " << PORT << std::endl;
        return false;
    }

    std::cout << "successfully created socket on port " << PORT << " TCP. listening..." << std::endl;
     
    */
    // add the listener to the master set
    FD_SET(listener, &master);
    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    return true;
}

bool Sock::create_server() {

    /* clear the master and temp sets */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
 
    /* get the listener */
    if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr << "cannot open socket\n";
        return false;
    }
    
    int yes = 1; // for setsockopt() SO_REUSEADDR, below

    /*"address already in use" error message */
    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        std::cerr << "cannot set socket opts\n";
        return false;
    }

    /* bind */
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(PORT);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //memset(&(localAddr.sin_zero), '\0', 8);
 
    if(bind(listener, (struct sockaddr *)&localAddr, sizeof(localAddr)) == -1)
    {
        std::cerr << "cannot bind port number " << PORT << std::endl;
        return false;
    }
 
    /*--- make into listener with SLOTS slots ---*/
    if(listen(listener, SLOTS) == -1)
    {
        std::cerr << "cannot listen to port number " << PORT << std::endl;
        return false;
    }

    std::cout << "successfully created socket on port " << PORT << " TCP. listening..." << std::endl;
     
    /* add the listener to the master set */
    FD_SET(listener, &master);
    /* keep track of the biggest file descriptor */
    fdmax = listener; /* so far, it's this one*/

    return true;
}

void Sock::snd(const NetEvent& serial) //, std::string servName = "")
{
    std::ostringstream os;
    boost::archive::text_oarchive oa(os);
    oa << serial;
    std::string message = os.str();
    
    if(send(listener, message.c_str(), message.size(),0) == -1) {
        std::cout << "cannot send data " << std::endl;
        close(listener);
        return;
    }
/*
bzero(buf,MAXMSG);
    nbytes = write(listener,buf,strlen(buf));
    if (nbytes < 0) {
        std::cerr << "ERROR writing to socket\n";
        return false;
    }
*/
}

bool Sock::rcv(NetEvent& serial, std::string servName)
{
    
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = 0;

if (servName != "") {
    bzero(buf,MAXMSG);
    
    /* copy it */
    read_fds = master;

    std::cout << "preparing to recieve data\n";
 
    if(select(fdmax+1, &read_fds, NULL, NULL, &tv) == -1)
    {
        std::cerr << "Client-select() error lol!\n";
        return false;
    }
    
    std::cout << "recieving data\n";

    if(!FD_ISSET(listener, &read_fds)) {
        std::cerr << "socket not ready for reading\n";
        return false;
    }


/*
    if (read(listener,buf,MAXMSG) == -1) {
        std::cerr << "ERROR reading from socket\n";
        return false;
    }
*/
    /* handle data from a client */
    if((nbytes = recv(listener, buf, sizeof(buf), 0)) <= 0)
    {
        /* got error or connection closed by client */
        if(nbytes == 0) {
        /* connection closed */
            std::cerr << "socket " << i << "hung up\n";
            return false;
        }
        else {
            std::cerr <<"recv() error lol!\n";
            return false;
        }
        /* close it... */
        close(i);
        /* remove from master set */
        FD_CLR(i, &master);
    }

    /* print received message */
    std::cout << "echo from " << inet_ntoa(remoteAddr.sin_addr)
              << " TCP:" << ntohs(remoteAddr.sin_port)
              << ": " << buf << std::endl;
    
    std::istringstream is(std::string(buf, strlen(buf)));
    boost::archive::text_iarchive ia(is);
    ia >> serial;
    return true;
}

    std::cout << "im the server\n";
    /* copy it */
    read_fds = master;
 
    std::cout << "preparing select\n";
    if(select(fdmax+1, &read_fds, NULL, NULL, &tv) == -1)
    {
        std::cerr << "Server-select() error lol!\n";
        return false;
    }
    printf("Server-select() is OK...\n");
     
    /*run through the existing connections looking for data to be read*/
    for(i = 0; i <= fdmax; i++)
    {
        if(FD_ISSET(i, &read_fds))
        { /* we got one... */
            std::cout << "preparing to recieve data\n";
            if(i == listener)
            {
                /* handle new connections */
                remoteLen = sizeof(remoteAddr);
                if((newfd = accept(listener, (struct sockaddr *)&remoteLen, &remoteLen)) == -1)
                {
                    std::cerr << "Server-accept() error lol!\n";
                    return false;
                }
                else
                {
                    printf("Server-accept() is OK...\n");
     
                    FD_SET(newfd, &master); /* add to master set */
                    if(newfd > fdmax)
                    { /* keep track of the maximum */
                        fdmax = newfd;
                    }
                    printf(":New connection from %s on socket %d\n", inet_ntoa(remoteAddr.sin_addr), newfd);
                    return false;
                }
            }
            else
            {
                /* handle data from a client */
                if((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
                {
                    /* got error or connection closed by client */
                    if(nbytes == 0)
                    /* connection closed */
                         printf(": socket %d hung up\n", i);
                    else
                        perror("recv() error lol!");
                    /* close it... */
                    close(i);
                    /* remove from master set */
                    FD_CLR(i, &master);
                }
                else
                {
                    /* we got some data from a client*/
                    for(j = 0; j <= fdmax; j++)
                    {
                        /* send to everyone! */
                        if(FD_ISSET(j, &master))
                        {
                           /* except the listener and ourselves */
                           if(j != listener && j != i)
                           {
                              if(send(j, buf, nbytes, 0) == -1)
                                  perror("send() error lol!");
                           }
                        }
                    }
                    /* print received message */
                    std::cout << "echo from " << inet_ntoa(remoteAddr.sin_addr)
                              << " TCP:" << ntohs(remoteAddr.sin_port)
                              << ": " << buf << std::endl;
        
                    std::istringstream is(std::string(buf, strlen(buf)));
                    boost::archive::text_iarchive ia(is);
                    ia >> serial;
                    return true;
                }
            }
            std::cout << "returning false\n";
            return false;
        }
    }
    std::cout << "returning false\n";
    return false;
}
/*
int main(int argc, char *argv[])
{
    Sock Socket;
    NetEvent nevent;
    nevent.is_down() = false;
    if ( argv[1] == NULL ) {
        std::cout << "initializing server ..\n";
        if ( !Socket.create_server() )
            return 1;
        for(;;)
        {
            Socket.rcv(nevent);
        }
    }
    else {
        std::cout << "initializing client ..\n";
        if ( !Socket.create_client(argv[1]) )
            return 1;
        for(;;)
        {
            Socket.snd(nevent);
            Socket.rcv(nevent,argv[1]);
            SDL_Delay(2000);
        }
    }
    return 0;
}
*/
