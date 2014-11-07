//client.cpp
 
/*
 * g++ -o client client.cpp or make
 * ./client <server-ip> port# username
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
#include <iostream>
#include "duckchat.h"
#define BUFLEN 1024
#define STDIN 0
 
int connectToSocket(char*, char*);//socket setup
int logUserIn(char*);//send login request and join Common
void sendReqs();//send requests to server
int readMessageType(struct text *r, int b);//reads what type of message recieved from server
int sayMess(struct text_say* rs);
int listMess(struct text_list* rl);
int whoMess(struct text_who* rw);
int errorMess(struct text_error* re);
void err(char*);//error function*/

struct addrinfo *addrAr;
int sockfd;
using namespace std;
int main(int argc, char* argv[])
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

    struct text* response;//for testing

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

        int bal = 0;
        if(bal = recvfrom(sockfd, buf, BUFLEN, 0,(struct sockaddr*)&addrAr->ai_addr, &addrAr->ai_addrlen) > 0)//if we recieve somthing, print it.
        {
            printf("recv()'d %d bytes of data in buf\n", bal);
            response = (text*) buf;
            readMessageType(response, bal);

        }
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

int sayMess(text_say *rs)
{
    return 0;
}

int listMess(text_list *rl)
{
    return 0;
}

int whoMess(text_who *rw)
{
    return 0;
}

int errorMess(text_error *re)
{
    return 0;
}

int readMessageType(struct text *r, int b) //what type of message have we recieved?
{
    int fin = 0;
    int netHost = 0;
    netHost = ntohl(r->txt_type);
    //check if addres is a crazy number or normal
    if(netHost > 3 || netHost < 0) {
       netHost = r->txt_type;
    }
    //check if request address is valid
    /*if(netHost != 0) {
        if(checkValidAddr(r) == -1) {
            //bad address, return
            cout << "invalid address\n";
            return -1;
        }
    }*/
    switch(netHost) {
    //printf("the value isss: %s \n", ntohl(r->req_type));
        case TXT_SAY:
            if(sizeof(struct text_say) == b) {
                cout << "say message\n";
                fin = sayMess((struct text_say*) r);
                break;
            } else {
                cout << "say message FAILED\n";
                break;
            }
        case TXT_LIST:
            if(sizeof(struct text_list) == b) {
                cout << "list message\n";
                fin = listMess((struct text_list*) r);
                break;
            } else {
                cout << "list message bad size\n";
                break;
            }
        case TXT_WHO:
            //printf("join case made \n");
            if(sizeof(struct text_who) == b) {
                cout << "who message\n";
                fin = whoMess((struct text_who*) r);
                break;
            } else {
                cout << "who message bad size\n";
                break;
            }
        case TXT_ERROR:
            if(sizeof(struct text_error) == b) {
                cout << "error message\n";
                fin = errorMess((struct text_error*) r);
                break;
            } else {
                cout << "error message bad size\n";
                break;
            }
        default:
            cout << "default case hit!!\n";
    }
    return fin;
}

