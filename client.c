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
#define STDIN 0
 
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
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(STDIN, &readfds);
    //error checking correct run input
    if(argc != 4)
    {
      printf("Specify ip port username\n");
      exit(0);
    }
    //call to connect socket and call to send login request to server
    connectToSocket(argv[1], argv[2]);
    logUserIn(argv[3]);
	printf("argv 1: %s argv 2: %s argv 3: %s\n", argv[1], argv[2], argv[3]);
	FD_SET(sockfd, &readfds);//add our socket to our set of files to read from

    while(1)
    {
    //We want to get array of request structs  
    //then evaluate request and have different method handle each request
	//call select to read from stdin and sock at same time
	if(select(sockfd+1, &readfds, NULL, NULL, NULL) == -1){
		perror("select\n");
		exit(4);
	}//end select
	
	
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
    //send login request to server and then send join common request
	printf("logging in...\n");
	//setting up structures
	struct request_login login;
	login.req_type = REQ_LOGIN;
	strcpy(login.req_username, user);
	struct request_join join;
	join.req_type = REQ_JOIN;
	strcpy(join.req_channel, "Common");
	printf("attempting to send login_req\n");
	//sending structs
	if (sendto(sockfd, &login, sizeof(login), 0, (struct
 		sockaddr*)addrAr->ai_addr, addrAr->ai_addrlen)==-1)
            err("sendto()");

	printf("attempting to send join_req\n");
	if (sendto(sockfd, &join, sizeof(join), 0, (struct
 		sockaddr*)addrAr->ai_addr, addrAr->ai_addrlen)==-1)
            err("sendto()");
	
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
