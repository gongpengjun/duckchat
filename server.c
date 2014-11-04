//UDPServer.c
 
/*
 *  gcc -o server UDPServer.c
 *  ./server
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include "duckchat.h"
#define BUFLEN 1024

const char* const REQ_STR[8] = {"REQ_LOGIN",
                                "REQ_LOGOUT",
                                "REQ_JOIN",
                                "REQ_LEAVE",
                                "REQ_SAY",
                                "REQ_LIST",
                                "REQ_WHO",
                                "REQ_KEEP_ALIVE"
                                };
const char* const TXT_STR[4] = {"TXT_SAY",
                                "TXT_LIST",
                                "TXT_WHO",
                                "TXT_ERROR"
                                }; 

socklen_t fromlen;
struct sockaddr recAddr;
int sockfd;

int connectToSocket(char*, char*);
void err(char*);
void readRequestType(struct request*, int);
void sayReq(struct request_say*);
struct addrinfo *addrAr;
 

int main(int argc, char **argv)
{
    addrAr = NULL;
    sockfd = 0;
    char buf[BUFLEN];
    connectToSocket(argv[1], argv[2]);
    while(1)
    {
        //for multiple requests maybe
        struct request *requests = (struct request*) malloc(sizeof (struct request) + BUFLEN);  
        int bal = 0;
        bal = recvfrom(sockfd, requests, sizeof buf, 0, &recAddr, &fromlen);
        if(bal > 0) 
        {
            printf("recv()'d %d bytes of data in buf\n", bal);
            readRequestType(requests, bal);       
        }    
    }
    return 0;
}

void err(char *str)
{
    perror(str);
    exit(1);
}

void sayReq(struct request_say *rs)
{
    //here we can handle req_say
}

void readRequestType(struct request *r, int b) 
{
    printf("made it to method \n");
    switch(ntohl(r->req_type)) {
        case REQ_SAY:
            printf("switchhh case worrked\n");
            sayReq( (struct request_say*) r );
            break;
        case REQ_LOGIN:
        
            break;
        case REQ_LOGOUT:
          
            break;  
        case REQ_JOIN:
        
            break;    
        case REQ_LEAVE:
            
            break;
        case REQ_LIST:
            
            break;
        case REQ_WHO:
            
            break;
    }
}

int connectToSocket(char* ip, char* port)
{
    struct addrinfo addressTmp;
    memset(&addressTmp, 0, sizeof addressTmp);
    addressTmp.ai_family = AF_INET;
    addressTmp.ai_socktype = SOCK_DGRAM;
    addressTmp.ai_flags = AI_PASSIVE;
    int check = 0;
    if((check = getaddrinfo(ip, port, &addressTmp, &addrAr))!= 0)
    {
        err("Server : getaddrinfo() NOT successful\n");
        return false;
    }
    if((sockfd = socket(addrAr->ai_family, addrAr->ai_socktype, addrAr->ai_protocol)) == -1)
    {
        err("Server : socket() NOT successful\n");
        return false;
    }
    if(bind(sockfd, addrAr->ai_addr, addrAr->ai_addrlen) == -1)
    {
        err("Server : bind() NOT successful\n");
        return false;
    }
    printf("socket and bind successful! \n");
    return true;
}
