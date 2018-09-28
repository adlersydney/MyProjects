#ifndef CLIENT_H
#define CLIENT_H

#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<string>
#include<cerrno>
#include<cstring>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<signal.h>
#include<algorithm>
#include<netdb.h>
#include<thread>
#include<random>

using namespace std;

class Client
{
  
public:
  int receiveFile(/*idk*/);
  int sendMessage(FILE* file);
  int sendFile();
  int verify();
  int createSocket();
  
private:
  sockaddr_in theClient;
  short port;
  FILE* connection;
  string fileName;
  unsigned short requestSize;
  int mySocket;
  hostent *host;
};

#end if
