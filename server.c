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
#define BUFLEN 512
#define PORT 9930

int connectToSocket(char*, char*);

struct addrinfo *addrAr;
 
void err(char *str)
{
    perror(str);
    exit(1);
}
 
int main(int argc, char **argv)
{
    addrAr = NULL;
    struct sockaddr_in my_addr, cli_addr;
    int sockfd, i;
    socklen_t slen=sizeof(cli_addr);
    char buf[BUFLEN];
    
	//adding struct
	struct request_say message;
    connectToSocket(argv[1], argv[2]);
    // if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    //   err("socket");
    // else
    //   printf("Server : Socket() successful\n");
 
    // bzero(&my_addr, sizeof(my_addr));
    // my_addr.sin_family = AF_INET;
    // my_addr.sin_port = htons(PORT);
    // my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     
    // if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)
    //   err("bind");
    // else
    //   printf("Server : bind() successful\n");
 
    while(1)
    {
        //if (recvfrom(sockfd, buf, BUFLEN, 0, (struct
	// if (recvfrom(sockfd, &message, sizeof(message), 0, (struct
 // sockaddr*)&cli_addr, &slen)==-1)
 //            err("recvfrom()");
 //        printf("Received packet from %s:%d\nData: %s\n\n",
 //               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), message.req_text);
     }
 
    //close(sockfd);
    return 0;
}

int connectToSocket(char* ip, char* port)
{
    struct addrinfo addressTmp;
    memset(&addressTmp, 0, sizeof addressTmp);
    addressTmp.ai_family = AF_INET;
    addressTmp.ai_socktype = SOCK_DGRAM;
    addressTmp.ai_flags = AI_PASSIVE;
    int check, secCheck = 0;
    if((check = getaddrinfo(ip, port, &addressTmp, &addrAr))!= 0)
    {
        err("Server : getaddrinfo() NOT successful\n");
        return false;
    }
    if((check = socket(addrAr->ai_family, addrAr->ai_socktype, addrAr->ai_protocol)) == -1)
    {
        err("Server : socket() NOT successful\n");
        return false;
    }
    if(bind(check, addrAr->ai_addr, addrAr->ai_addrlen) == -1)
    {
        err("Server : bind() NOT successful\n");
        return false;
    }
    return true;
}
