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
map<string, string> addrToUser;
map<string, string> userToAddr;
map<string,vector<string>> usrLisChan;
map<string,string> usrTlkChan;
map<string,vector<string>> chanTlkUser;
vector<string> channels;
//methods
int connectToSocket(char*, char*);
void err(char*);
int readRequestType(struct request*, int);
int sayReq(struct request_say*);
int checkValidAddr();
string getReqAddr();

//program
int main(int argc, char **argv)
{
    addrAr = NULL;
    sockfd = 0;
    connectToSocket(argv[1], argv[2]);
    while(1)
    {
        //for multiple requests maybe
        // requests = (struct request*) malloc(sizeof (struct request) + BUFLEN); 
        char *buf = new char[BUFLEN];
	    struct request *requests;  
        int bal = 0;
        bal = recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&recAddr, &fromlen);
        if(bal > 0) {
            printf("recv()'d %d bytes of data in buf\n", bal);
            requests = (request*) buf;
            readRequestType(requests, bal);       
        } 
       requests = NULL;
       delete[] buf;   
    }
    return 0;
}
//returns string of username of current request address
string getReqAddr()
{
    //new request address info
    struct sockaddr_in* address = (struct sockaddr_in*)&recAddr;   
    char *addrString = (char*)malloc(sizeof(char)*BUFLEN);
    //make address string
    inet_ntop(AF_INET, &address, addrString, BUFLEN);
    //have tmp var
    string realAddrString = addrString;
    free (addrString);
    string aTmp = addrToUser[realAddrString];
    return aTmp;

}
//check if current request address is valid or exist in map
int checkValidAddr(struct request *r) 
{
    //new request address info
    struct sockaddr_in* address = (struct sockaddr_in*)&recAddr;   
    char *addrString = (char*)malloc(sizeof(char)*BUFLEN);
    //make address string
    inet_ntop(AF_INET, &address, addrString, BUFLEN);
    //have tmp var
    string realAddrString = addrString;
    free (addrString);
    //look in map for address
    string aTmp = addrToUser[realAddrString];
    if(aTmp != "") {
        //valid addr and user
        //may need to change addr
        return 0;
    } else {
        return -1;
    }
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
    /*
    get username of request
    get channel of request

    */
    return 0;
}
//handle login requests
int loginReq(struct request_login *rl)
{
    //new request address info
    struct sockaddr_in* address = (struct sockaddr_in*)&recAddr;
    //username
    string username = rl->req_username;
    char *addrString = (char*)malloc(sizeof(char)*BUFLEN);
    //make address string
    inet_ntop(AF_INET, &address, addrString, BUFLEN);
    //have tmp var
    string realAddrString = addrString;
    free (addrString);
    //look in map for address
    string aTmp =  addrToUser[realAddrString];
    if(aTmp == "") {
        printf("New User: %s \n", username);
        addrToUser[realAddrString] = username;
        userToAddr[username] = realAddrString;
        return 1;
    } else {
        printf("Old User: %s \n", username);
        addrToUser.erase(aTmp);
        addrToUser[realAddrString] = username;
        userToAddr[username] = realAddrString;
        return 0;
    }
    return 0;
}
//handle login requests
int logoutReq(struct request_logout *rl)
{
    //new request address info
    struct sockaddr_in* address = (struct sockaddr_in*)&recAddr;
    char *addrString = (char*)malloc(sizeof(char)*BUFLEN);
    //make address string
    inet_ntop(AF_INET, &address, addrString, BUFLEN);
    //have tmp var
    string realAddrString = addrString;
    free (addrString);
    string user;
    //get username 
    user = addrToUser[realAddrString];
    map<string,string>::iterator it;
    //delete address and user in both maps
    it = addrToUser.find(realAddrString);
    addrToUser.erase(it);
    it = userToAddr.find(userToAddr);
    userToAddr.erase(it);
    //delete user and channel stuff
    it = usrLisChan.find(user);
    usrLisChan.erase(it);
    it = usrTlkChan.find(user);
    usrTlkChan.erase(it);
    return 0;
}
//handle login requests
int joinReq(struct request_join *rj)
{
    //create tmp vars for usename and channel of request
    string chan = (string)rj->req_channel;
    string user = getReqAddr();
    int trig = 0;
    //tmp vector for channel user is listening to
    vector<string> vTmpL = usrLisChan[user];
    //tmp string for channel user is talking to
    string sTmpT = usrTlkChan[user];
    //tmp vec for users on channel
    //vector<string> vTmpU = chanTlkUser[chan];
    //new channel case
    // for(int j=0; j<vTmpU.size(); j++) {
    //     if(vTmpU[j] == user) {
    //         printf("User is already in channel: %s \n", user );
    //         trig = 1;
    //     }
    // }
    // if(trig == 0) {
    //     //add user to channel in map
    //     printf("User is getting added to chanTlkUser: %s \n", user );
    //     vTmpU.push_back[user];
    // }
    //trig = 0;
    for(int i=0; i<channels.size(); i++) {
        if(channels[i] == chan) {
            trig = 1;
        }
    }
    if(trig == 0) {
        //add new channel
        printf("Channel is getting added to channels global array: %s \n", chan);
        channels.push_back(chan);
    }
    //add new channel to back
    vTmpL.push_back(chan);
    sTmpT = chan;
    //add vectors back to map, new channel at the back.
    usrLisChan[user] = vTmpL;
    usrTlkChan[user] = sTmpT;
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
    if(r->req_type > 6 || r->req_type < 0) {
            printf("[ERROR] Issue during recieve from client\n");
            return -1;
    }
    //check if request address is valid
    if(r->req_type != 0) {
        if(checkValidAddr(r) == -1) {
            //bad address, return
            printf("Error checking addres \n");
            return -1;
        } 
    }
    int netHost = 0;
    netHost = ntohl(r->req_type);
    if(netHost > 6 || netHost < 0) {
	   netHost = r->req_type;
    }

    switch(netHost) {
	//printf("the value isss: %s \n", ntohl(r->req_type));
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
		    //printf("join case made \n");
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
            printf("this is default: %d \n", ntohl(r->req_type));
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
