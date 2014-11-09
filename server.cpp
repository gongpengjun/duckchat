//UDPServer.c
 
/*  login = works
    logout = works
    
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
#include <utility>
#include <vector>
#include "duckchat.h"
//defined
#define BUFLEN 1024
using namespace std;
//globals
struct sockaddr recAddr;
socklen_t fromlen = sizeof(recAddr);
int sockfd;
struct addrinfo *addrAr;
multimap<pair<string,string>, string> addrToUser;
multimap<string, pair<string,string> > userToAddr;
map<string,string> usrTlkChan;
map<string,struct sockaddr_in> userToAddrStrct;
map<string,vector<string> > chanTlkUser;
vector<string> channels;
//methods
int connectToSocket(char*, char*);
void err(char*);
int readRequestType(struct request*, int);
int sayReq(struct request_say*);
int checkValidAddr();
string getUserOfCurrAddr();
string getAddr_string();
string getSemiAddr_string();
int getAddr_Port();
struct sockaddr_in getAddrStruct();
int checkAddrEq(struct sockaddr_in a, struct sockaddr_in b);

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
        struct request *requests = (struct request*)malloc(sizeof(struct request*) + BUFLEN);  
        int bal = 0;
        bal = recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&recAddr, &fromlen);
        if(bal > 0) {
            printf("recv()'d %d bytes of data in buf\n", bal);
            requests = (request*) buf;
            cout << "do I get herer??? \n";
            readRequestType(requests, bal);
        } 

       //requests = NULL;
       delete[] buf;   
    }
    return 0;
}
//returns string of username of current request address
string getUserOfCurrAddr()
{ 
    struct sockaddr_in* address = (struct sockaddr_in*)&recAddr;
    string aTmp = "";     
    map<string,struct sockaddr_in>::iterator i;
    for(i=userToAddrStrct.begin(); i != userToAddrStrct.end(); i++) {
        if(checkAddrEq(i->second,*address) == 0) {
            aTmp = i->first;
        }

    }
    return aTmp;
}
int checkAddrEq(struct sockaddr_in a, struct sockaddr_in b)
{
    char *addrA = (char*)malloc(sizeof(char)*BUFLEN);
    char *addrB = (char*)malloc(sizeof(char)*BUFLEN);
    inet_ntop(AF_INET, &(a.sin_addr), addrA, BUFLEN);
    inet_ntop(AF_INET, &(b.sin_addr), addrB, BUFLEN);
    string stringA = addrA;
    string stringB = addrB;
    if(stringA == stringB) {
        int portA = a.sin_port;
        int portB = a.sin_port;
        if(portA == portB) {
            return 0;
        }
    }
    return -1;
}

struct sockaddr_in getAddrStruct() 
{
    struct sockaddr_in* address = (struct sockaddr_in*)&recAddr;
    return *address;
}
//check if current request address is valid or exist in map
int checkValidAddr(struct request *r) 
{
    struct sockaddr_in* address = (struct sockaddr_in*)&recAddr;
    map<string,struct sockaddr_in>::iterator i;
    for(i=userToAddrStrct.begin(); i != userToAddrStrct.end(); i++) {
        if(checkAddrEq(i->second,address)==0) {
            return 1;
        }
    }
    return 0;
}
//handle say requests
int sayReq(struct request_say *rs)
{
    string channel = rs->req_channel;
    string message = rs->req_text;
    string username = getUserOfCurrAddr();
    cout << "Username fromuser: " << username << "\n";
    map<string,vector<string> >::iterator hit = chanTlkUser.find(channel);
    if(hit == chanTlkUser.end()) {
        cout << "JESUS CHRIST: " << channel <<"\n";  
        return -1; 
    }
    vector<string> tmpU = hit->second;
    for(int i=0; i<tmpU.size(); i++) {
        cout << "user: " << tmpU[i] << " on channel: " << channel << " in iteration on say loop:  " << i <<"\n";
        struct sockaddr_in address;
        map<string, struct sockaddr_in>::iterator ui = userToAddrStrct.find(tmpU[i]);
        address = ui->second;
        struct text_say *msg;
        msg->txt_type= htonl(TXT_SAY);
        strncpy(msg->txt_username, username.c_str(), USERNAME_MAX);
        strncpy(msg->txt_text, message.c_str(), SAY_MAX);
        strncpy(msg->txt_channel, channel.c_str(), CHANNEL_MAX);
        int size = sizeof(struct sockaddr*);
        int res= sendto(sockfd, msg, sizeof(struct text_say), 0, (struct sockaddr*)&address, size);
        if (res == -1) {
            cout << "sendto very badd \n";
            //return -1;
        }
        free(msg);
    }
    tmpU.clear();
    return 0;
}
//handle login requests
int loginReq(struct request_login *rl)
{
    string username = rl->req_username;
    struct sockaddr_in strctAddr = getAddrStruct();
    userToAddrStrct[username] = strctAddr;
    map<string,vector<string> >::iterator it = chanTlkUser.find("Common");
    vector<string> usersC;
    if(it == chanTlkUser.end()) {
        chanTlkUser.insert(pair<string,vector<string> >("Common", usersC));
        channels.push_back("Common");
    }
    it = chanTlkUser.find("Common");
    usersC = it->second;
    usersC.insert(usersC.begin(), username);
    chanTlkUser["Common"] = usersC;
    usrTlkChan.insert(pair<string,string>(username, "Common"));    
    return 0;
}
//handle login requests
int logoutReq(struct request_logout *rl)
{
    string username = getUserOfCurrAddr();
    map<string, struct sockaddr_in>::iterator sockIt = userToAddrStrct.find(username);
    userToAddrStrct.erase(sockIt);
    
    // look for user in channel listen
    //look for user in channel talk

    map<string,string>::iterator git;
    git = usrTlkChan.find(username);
    if(git != usrTlkChan.end()) {
        cout << "deleting  5 \n";
        usrTlkChan.erase(username);
    }
    //erase user on channels in chanTlkUser
    for(int ick=0; ick<channels.size(); ick++) {
        cout << "in outerr \n";
        map<string,vector<string> >::iterator itck = chanTlkUser.find(channels[ick]);
        vector<string> usersC = itck->second;
        for(int j=0; j<usersC.size(); j++) {
            if(usersC[j] == username) {
                cout << "deleting user: " << usersC[j] << " \n";
                usersC.erase(usersC.begin()+j);
            }
        }
        chanTlkUser.erase(itck);
        chanTlkUser.insert(pair<string,vector<string> >(channels[ick],usersC));
    }
    return 0;
}
//handle login requests
int joinReq(struct request_join *rj)
{
    //create tmp vars for username and channel of request
    string chan = (string)rj->req_channel;
    string user = getUserOfCurrAddr();
    int trig = 0;
    map<string,vector<string> >::iterator it = chanTlkUser.find(chan);
    vector<string> usersC;
    if(it == chanTlkUser.end()) {
        //NEW CHANNEL
        usersC.insert(usersC.begin(), user);
        chanTlkUser.insert(pair<string,vector<string> >(chan, usersC));
        channels.push_back(chan);

    } else {
        //old channel
        it = chanTlkUser.find(chan);
        usersC = it->second;
        for(int i=0; i<usersC.size(); i++) {
            if(usersC[i] == user) {
                return -1;
            }
        }
        usersC.insert(usersC.begin(), user);
        chanTlkUser[chan] = usersC;
    }
    string chanTlk = usrTlkChan[user];
    chanTlk = chan;
    usrTlkChan[user] = chanTlk;
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
    struct sockaddr* address; 
    multimap<string, pair<string,string> >::iterator ui = userToAddr.find(getUserOfCurrAddr());
    pair<string,string> tad = ui->second;
    string ad = tad.second;
    char *s= (char*) malloc(sizeof(char)*BUFLEN);
    strncpy(s, ad.c_str(), strlen(ad.c_str()));
    inet_pton(AF_INET, s, &address);
    struct text_list *msg= (struct text_list*) malloc(sizeof(struct text_list) + BUFLEN);
    msg->txt_type= htonl(TXT_LIST);
    msg->txt_nchannels = htonl(channels.size());
    for (int i = 0; i < channels.size(); i++) {
        strncpy(msg->txt_channels[i].ch_channel, channels[i].c_str(), CHANNEL_MAX);
    }
    int size = sizeof(struct sockaddr);
    int res= sendto(sockfd, msg, sizeof(struct text_list), 0, address, size);
    if (res == -1) {
        cout << "sendto very badd \n";
        //return -1;
    }
    free(msg);
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
            cout << "invalid address\n";
            return -1;
        } 
    }
    switch(netHost) {
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