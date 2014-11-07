//UDPClient.c
 
/*
 * gcc -o client UDPClient.c
 * ./client <server-ip>
 */
 
#include <stdio.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <fcntl.h>
#include "duckchat.h"
#define BUFLEN 1024
#define STDIN 0
 
int connectToSocket(char*, char*);
//int isReadable(sd, int*, int timeOut);
int logUserIn(char*);
void sendReqs();
void err(char*);

struct addrinfo *addrAr;
int sockfd;

int main(int argc, char** argv)
{
    //initialize vars
	char buf[BUFLEN];
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
	printf("argv 1: %s argv 2: %s argv 3: %s\n", argv[1], argv[2], argv[3]);
	//FD_SET(sockfd, &readfds);//add our socket to our set of files to read from

	struct text_say response;//for testing

    while(1)
    {
    	//We want to get array of request structs  
    	//then evaluate request and have different method handle each request
	fd_set readfds;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO(&readfds);//clean set
	FD_SET(STDIN, &readfds);//add stdin

	//call select to read from stdin if there is input available
	if(select(STDIN+1, &readfds, NULL, NULL, &tv) == -1){
		perror("select\n");
		exit(4);
	}//end select

	//printf("\nEnter data to send(Type exit and press enter to exit) :");
	if(FD_ISSET(STDIN, &readfds))
	{
		FD_CLR(STDIN, &readfds);
		scanf("%[^\n]",buf);
		getchar();
		if(strcmp(buf,"exit") == 0)
		  exit(0);
	}
        
	if(recvfrom(sockfd, &response, sizeof(response), 0,(struct sockaddr*)&addrAr->ai_addr, &addrAr->ai_addrlen)>0)//if we recieve somthing, print it.
	printf("Response: %s\n", response.txt_text);
    
	}
}//end main

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
	//sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(sockfd == -1)
        {
            err("Client : socket() NOT successful\n");
            continue;
        }
        break;
    }
    addrAr = tmpAdAr;
    /*int flags = fcntl(sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);*/
    fcntl(sockfd, F_SETFL, O_NONBLOCK);//set socket to be non-blocking
    printf("socket and bind successful! \n");
    return true;
}

int isReadable(int sd, int* error, int timeOut)
{
	fd_set socketReadSet;
	FD_ZERO(&socketReadSet);
	FD_SET(sd,&socketReadSet);
	struct timeval tv;
	if (timeOut){
		tv.tv_sec  = timeOut / 1000;
		tv.tv_usec = (timeOut % 1000) * 1000;
	}
	else {
		tv.tv_sec  = 0;
		tv.tv_usec = 0;
	} // if
	if (select(sd+1,&socketReadSet,0,0,&tv) == -1) {//SOCKET_ERROR
		*error = 1;
		return 0;
	} // if
	*error = 0;
	return FD_ISSET(sd,&socketReadSet) != 0;
}
