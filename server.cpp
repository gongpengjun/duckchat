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
#include <stdbool.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "duckchat.h"
//defined
#define BUFLEN 1024
using namespace std;
//globals
socklen_t fromlen;
struct sockaddr recAddr;
int sockfd;
struct addrinfo *addrAr;
map<string,int> usrLisChan;
map<string,int> usrtlkChan;
vector<string> users;
//methods
int connectToSocket(char*, char*);
void err(char*);
int readRequestType(struct request*, int);
int sayReq(struct request_say*);

//program
int main(int argc, char **argv)
{
    addrAr = NULL;
    sockfd = 0;
    char buf[REQ_MAX];
    connectToSocket(argv[1], argv[2]);
    while(1)
    {
        //for multiple requests maybe
        // requests = (struct request*) malloc(sizeof (struct request) + BUFLEN); 
        struct request *requests;  
        int bal = 0;
        printf("joe says print here\n");
        bal = recvfrom(sockfd, buf, REQ_MAX, 0, &recAddr, &fromlen);
        if(bal > 0) {
            printf("recv()'d %d bytes of data in buf\n", bal);
            requests = (request*) buf;
            readRequestType(requests, bal);       
        } 
        free(requests);   
    }
    return 0;
}
//for errors
void err(char *str)
{
    perror(str);
    exit(1);
}
//handle say requests
int sayReq(struct request_say *rs)
{
    return 0;
}
//handle login requests
int loginReq(struct request_login *rl)
{
    printf("user name: %s \n", rl->req_username);
    return 0;
}
//handle login requests
int logoutReq(struct request_logout *rl)
{
    return 0;
}
//handle login requests
int joinReq(struct request_join *rj)
{
    return 0;
}
//handle login requests
int leaveReq(struct request_leave *rl)
{
    return 0;
}
//handle login requests
int listReq(struct request_list *rl)
{
    return 0;
}
//handle login requests
int whoReq(struct request_who *rw)
{
    return 0;
}

int readRequestType(struct request *r, int b) 
{
    int fin = 0;
    printf("made it to method \n");
    //check if the user is logged in
    if(r->req_type > 6 || r->req_type < 0){
            printf("[ERROR] Issue during recieve from client\n");
            return;
    }
    switch(ntohl(r->req_type)) {
        case REQ_LOGIN:
            if(sizeof(struct request_login) == b) {
                printf("switchhh case login valid\n");
                fin = loginReq((struct request_login*) r);
                break;
            } else {
                printf("switchhh case login INvalid\n");
                break;
            } 
        case REQ_LOGOUT:
            if(sizeof(struct request_logout) == b) {
                printf("switchhh case logout valid\n");
                fin = logoutReq((struct request_logout*) r);
                break;
            } else {
                printf("switchhh case logout INvalid\n");
                break;
            }   
        case REQ_JOIN:
            if(sizeof(struct request_join) == b) {
                printf("switchhh case join valid\n");
                fin = joinReq((struct request_join*) r);
                break;
            } else {
                printf("switchhh case join INvalid\n");
                break;
            }      
        case REQ_LEAVE:
            if(sizeof(struct request_leave) == b) {
                printf("switchhh case leave valid\n");
                fin = leaveReq((struct request_leave*) r);
                break;
            } else {
                printf("switchhh case leave INvalid\n");
                break;
            }
        case REQ_SAY:
            if(sizeof(struct request_say) == b) {
                printf("switchhh case say valid\n");
                fin = sayReq((struct request_say*) r);
                break;
            } else {
                printf("switchhh case say INvalid\n");
                break;
            }
        case REQ_LIST:
            if(sizeof(struct request_list) == b) {
                printf("switchhh case list valid\n");
                fin = listReq((struct request_list*) r);
                break;
            } else {
                printf("switchhh case list INvalid\n");
                break;
            }
        case REQ_WHO:
            if(sizeof(struct request_who) == b) {
                printf("switchhh case who valid\n");
                fin = whoReq((struct request_who*) r);
                break;
            } else {
                printf("switchhh case who INvalid\n");
                break;
            }
        default:
            printf("Hit Default\n");
    }
    return fin;
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
