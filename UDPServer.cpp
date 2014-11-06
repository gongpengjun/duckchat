//UDPServer.c
 
/*
 *  gcc -o server UDPServer.c
 *  ./server
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <map>
#include "duckchat.h"
#define BUFLEN 64
#define PORT 9930
 
void err(char *str)
{
    perror(str);
    exit(1);
}
 
int main(void)
{
    struct sockaddr_in my_addr, cli_addr;
    int sockfd, i, clientcount = 1;
    socklen_t slen=sizeof(cli_addr);
    char buf[BUFLEN];
	char response[BUFLEN];
	//adding struct
	struct request_say message;
    std::map<char*, struct sockaddr_in> clients;
    std::map<char*, struct sockaddr_in>::iterator it = clients.begin();
    std::pair<std::map<char*, struct sockaddr_in>::iterator, bool> ret;
 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
      err("socket");
    else
      printf("Server : Socket() successful\n");
 
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     
    if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)
      err("bind");
    else
      printf("Server : bind() successful\n");
 
    while(1)
    {
        //if (recvfrom(sockfd, buf, BUFLEN, 0, (struct
	if (recvfrom(sockfd, &message, sizeof(message), 0, (struct
 sockaddr*)&cli_addr, &slen)==-1)
            err("recvfrom()");
        printf("Received packet from %s:%d\nData: %s\n\n",
               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), message.req_text);

        if(message.req_type = REQ)
	strcpy(response, message.req_text);
    //test char*
    char b = clientcount++ + 48;
    char* c = "default \n";
    if(clientcount >= 2)
    {
        clientcount = 1;
    }
    char* a = new char[strlen(c)+2];
    strcpy(a, c);
    a[strlen(c)-1] = b;
    a[strlen(c)+1] = '\0';
    printf("a = %s\n", a);

    ret = clients.insert(std::pair<char*, struct sockaddr_in>(a, cli_addr));
    if(ret.second == false)
    {
        printf("Message was from known client: %s\n",ret.first->first);
    }

	response[BUFLEN-1] = '\0';
    printf("before for\n");
    for(it = clients.begin(); it!= clients.end(); ++it)
    {
        //if(sendto(sockfd, response, BUFLEN, 0, (struct sockaddr*)&cli_addr,
        if(sendto(sockfd, response, BUFLEN, 0, (struct sockaddr*)&it->second,
            sizeof(it->second)) == -1)
            perror("sento() failed\n");
    }
    printf("after for\n");
    }


    close(sockfd);
    return 0;
}
