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
map<string,vector<string> > usrLisChan;
map<string,string> usrTlkChan;
map<string,vector<string> > chanTlkUser;
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
            /*
	    map<string,string>::iterator it;
            if(!addrToUser.empty()) {
		for(it = addrToUser.begin(); it != addrToUser.end(); it++) {
                    cout << it->first << " that is key.\n";
                    cout << it->second << " that is value.\n";
                }  
	    }
            map<string,string>::iterator its;
	    if(!userToAddr.empty()) {
                for(its = userToAddr.begin(); its != userToAddr.end(); its++) {
                    cout << its->first << " that is key.\n";
                    cout << its->second << " that is value.\n";
                }
	    } */   
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
int sayReq(struct text_say *rs)
{
    /*
    get username of request
    get channel of request*/
    string username = rs->txt_username;
    string channel = rs->txt_channel;
    string message = rs->txt_text;
    //get list of users on channel from usrLisChan
    vector<string> tmpU = usrLisChan[username];
    //for all users on the channel
        //write the message to those users by there address
    for(int i=0; i<tmpU.size(); i++) {
        cout << "user: " << tmpU[i] << " on channel: " << channel << "\n";
        //get address of current user
        struct sockaddr_in* address = (struct sockaddr_in*)&recAddr;
        char* ad = (char*)userToAddr[tmpU[i]];
        inet_pton(AF_INET, ad, &address);
        struct text_say *msg= (struct text_say*) malloc(sizeof(struct text_say));
        msg->txt_type= htonl(TXT_SAY);
        msg->txt_username = (char*)tmpU[i];
        msg->txt_text = (char*)message;
        int res= sendto(sockfd, msg, sizeof(struct text_say), 0, address, sizeof(struct sockaddr_in));
        if (res == -1) {
            cout << "sendto very badd \n";
            return -1;
        }
        free(msg);
    }
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
        cout << "new User\n";
	    addrToUser[realAddrString] = username;
        userToAddr[username] = realAddrString;
        return 1;
    } else {
        cout << "old user\n";
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
    //it.clear();
    map<string,string>::iterator its;
    its = userToAddr.find(user);
    userToAddr.erase(its);
    //delete user and channel stuff
    //it = usrLisChan.find(user);
    //usrLisChan.erase(it);
    //it = usrTlkChan.find(user);
    //usrTlkChan.erase(it);
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
    vector<string> vTmpU = chanTlkUser[chan];
    //new channel case
    for(int j=0; j<vTmpU.size(); j++) {
        if(vTmpU[j] == user) {
            cout << "user in channel already\n";
            trig = 1;
        }
    }
    if(trig == 0) {
        //add user to channel in map
        cout << "user gets added to channel map\n";
        vTmpU.push_back[user];
    }
    trig = 0;
    for(int i=0; i<channels.size(); i++) {
        if(channels[i] == chan) {
            trig = 1;
        }

    }
    if(trig == 0) {
        //add new channel
       // printf("Channel is getting added to channels global array: %s \n", chan);
         cout << "Channel is gettting added to glob array\n";
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
    int netHost = 0;
    netHost = ntohl(r->req_type);
    //check if addres is a crazy number or normal
    if(netHost > 6 || netHost < 0) {
       netHost = r->req_type;
    }
    //check if request address is valid
    if(netHost != 0) {
        if(checkValidAddr(r) == -1) {
            //bad address, return
            cout << "invalid address\n";
            return -1;
        } 
    }
    switch(netHost) {
	//printf("the value isss: %s \n", ntohl(r->req_type));
        case REQ_LOGIN:
            if(sizeof(struct request_login) == b) {
                cout << "login request\n";
                fin = loginReq((struct request_login*) r);
                break;
            } else {
                cout << "login request FAILED\n";
                break;
            } 
        case REQ_LOGOUT:
            if(sizeof(struct request_logout) == b) {
                cout << "logout request\n";
                fin = logoutReq((struct request_logout*) r);
                break;
            } else {
                cout << "logout request bad size\n";
                break;
            }   
        case REQ_JOIN:
		    //printf("join case made \n");
            if(sizeof(struct request_join) == b) {
                cout << "join request\n";
                fin = joinReq((struct request_join*) r);
                break;
            } else {
                cout << "switch request bad size\n";
                break;
            }      
        case REQ_LEAVE:
            if(sizeof(struct request_leave) == b) {
                cout << "leave request\n";
                fin = leaveReq((struct request_leave*) r);
                break;
            } else {
                cout << "leave request bad size\n";
                break;
            }
        case REQ_SAY:
            if(sizeof(struct request_say) == b) {
                cout << "say request\n";
                fin = sayReq((struct request_say*) r);
                break;
            } else {
                cout << "say request bad size\n";
                break;
            }
        case REQ_LIST:
            if(sizeof(struct request_list) == b) {
                cout << "list request\n";
                fin = listReq((struct request_list*) r);
                break;
            } else {
                cout << "list request bad size\n";
                break;
            }
        case REQ_WHO:
            if(sizeof(struct request_who) == b) {
                cout << "who request\n";
                fin = whoReq((struct request_who*) r);
                break;
            } else {
                cout << "who request bad size\n";
                break;
            }
        default:
            cout << "default case hit!!\n";
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
        cout << "Server : getaddrinfo() NOT successful \n";
        return false;
    }
    if((sockfd = socket(addrAr->ai_family, addrAr->ai_socktype, addrAr->ai_protocol)) == -1)
    {
        cout << "Server : socket() NOT successful \n";
        return false;
    }
    if(bind(sockfd, addrAr->ai_addr, addrAr->ai_addrlen) == -1)
    {
        cout << "Server : bind() NOT successful \n";
        return false;
    }
    cout << "socket and bind successful! \n";
    return true;
}
