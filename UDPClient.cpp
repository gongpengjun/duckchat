//UDPClient.c
 
/*
 * gcc -o client UDPClient.c
 * ./client <server-ip>
 */
 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "duckchat.h"
#include <iostream>
#include <map>
#define BUFLEN 64 //changed this from 512
#define PORT 9930
 
void err(char *s)
{
    perror(s);
    exit(1);
}
 
int main(int argc, char** argv)
{
    struct sockaddr_in serv_addr;
    int sockfd, i;
    socklen_t slen=sizeof(serv_addr);
    char buf[BUFLEN];
	//adding struct
	struct request_say message;
 
    if(argc != 2)
    {
      printf("Usage : %s <Server-IP>\n",argv[0]);
      exit(0);
    }
 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        err("socket");
 
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_aton(argv[1], &serv_addr.sin_addr)==0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
 
    while(1)
    {
        printf("\nEnter data to send(Type exit and press enter to exit) : ");
        scanf("%[^\n]",buf);
        getchar();
        if(strcmp(buf,"exit") == 0)
          exit(0);
	//adding struct
	printf("before set\n");
	message.req_type = REQ_SAY;
	printf("before strcpy\n");
	strcpy(message.req_channel, "default\n");
	strcpy(message.req_text, buf);
 
        //if (sendto(sockfd, buf, BUFLEN, 0, (struct
	printf("attempting to send message\n");
	if (sendto(sockfd, &message, sizeof(message), 0, (struct
 		sockaddr*)&serv_addr, slen)==-1)
            err("sendto()");

    if (recvfrom(sockfd, buf, BUFLEN, 0,(struct sockaddr*)&serv_addr, &slen) == -1)
        	perror("recvfrom() failed\n");
	printf("Response: %s\n", buf);
    }
 
    close(sockfd);
    return 0;
}
 
//</server-ip>
