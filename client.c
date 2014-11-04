//UDPClient.c
 
/*
 * gcc -o client UDPClient.c
 * ./client <server-ip>
 */
 
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
 
int connectToSocket(char*, char*);
int logUserIn(char*);
void err(char*);

struct addrinfo *addrAr;
int sockfd;

int main(int argc, char** argv)
{
    //initialize vars
    addrAr = NULL;
    sockfd = 0;
    //error checking correct run input
    if(argc != 4)
    {
      printf("Specify ip port username\n");
      exit(0);
    }
    //call to connect socket and call to send login request to server
    connectToSocket(argv[1], argv[2]);
    logUserIn(argv[3]);

    while(1)
    {
    //We want to get array of request structs  
    //then evaluate request and have different method handle each request
    }
 
    return 0;
}

void err(char *s)
{
    perror(s);
    exit(1);
}

int logUserIn(char* user)
{
    //send login request to server
    return true;
}

int connectToSocket(char* ip, char* port)
{
    struct addrinfo addressTmp;
    struct addrinfo *tmpAdAr;
    memset(&addressTmp, 0, sizeof addressTmp);
    addressTmp.ai_family = AF_INET;
    addressTmp.ai_socktype = SOCK_DGRAM;
    int check = 0;
    if((check = getaddrinfo(ip, port, &addressTmp, &addrAr))!= 0)
    {
        err("Client : getaddrinfo() NOT successful\n");
        return false;
    }
    for(tmpAdAr = addrAr; tmpAdAr != NULL; tmpAdAr = tmpAdAr->ai_next)
    {
        sockfd = socket(tmpAdAr->ai_family, tmpAdAr->ai_socktype, tmpAdAr->ai_protocol);
        if(sockfd == -1)
        {
            err("Client : socket() NOT successful\n");
            continue;
        }
        break;
    }
    addrAr = tmpAdAr;
    
    printf("socket and bind successful! \n");
    return true;
}
