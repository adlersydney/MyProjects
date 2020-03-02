/*
  CSE109
  Sydney Adler
  sea220
  Program 5
*/

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
#include<fcntl.h>
#include"Hash.h"

using namespace std;

ssize_t safeWrite(int fd, const void *buf, size_t count);
ssize_t safeRead(int fd, void *buf, size_t count);

int main(int argc, char** argv)
{
  short portNum;
  int mySocket;
  if((mySocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      cerr << "Socket failed";
      exit(1);
    }

  sockaddr_in client;
  memset(&client, 0, sizeof(sockaddr_in));
  
   char myFile[]  = "connection.txt";
   int fd = open(myFile, O_RDONLY);

   if(fd == -1)
     {
       cerr << "Open connection.txt failed.";
     }
   
   int hostLen;
   int servedSize;
   char hostName[1024];
   
   read(fd, &portNum, 2);
   read(fd, &hostLen, 4);;
   read(fd, hostName, hostLen);
   read(fd, &servedSize, 4);
   char* served = (char*)malloc(servedSize + 1);
   read(fd, served, servedSize);
   close(fd);
   
   client.sin_family = AF_INET;
   client.sin_port = htons(portNum);

   hostent* host = gethostbyname("localhost");
   if (host == NULL)
     {
       cerr << "Cannot get host name.";
       exit(8);
     }

   int connected = connect(mySocket, (sockaddr*)&client, sizeof(sockaddr_in));
   if (connected == -1)
     {
       cerr<< " Could not connect" << endl;
       exit(9);
     }
   
   unsigned short dataSize = 11;
   int amount = 5;
   int start = 30;
   safeWrite(connected, &dataSize, 2);
   safeWrite(connected, &start, 4);
   safeWrite(connected, &amount, 4);
   int msgSize, rqstSize;

   while(safeRead(connected, &msgSize, 4) == -1)
     {
       continue;
     }

   safeRead(connected, &rqstSize, 4);
   char* dataRequest = (char*)malloc(rqstSize);
   safeRead(connected, dataRequest, rqstSize);
   char* check = (char*)malloc(msgSize);
   safeRead(connected,check, (msgSize-rqstSize-8));
   unsigned char* integerCheck = Hash(dataRequest, rqstSize);
   if (strcmp((const char*)integerCheck, check) != 0)
     {
       cerr << "The hash integrity check failed.";
     }

   free(served);
   served = NULL;
   free(dataRequest);
   dataRequest = NULL;
   free(check);
   check = NULL;
   
}

ssize_t safeWrite(int fd, const void *buf, size_t count)
{
  size_t amt = count;
  size_t amtRead = 0;
  char* charCast = NULL;
  ssize_t result = write(fd, charCast, amt);
  while(amt)
    {
      charCast = ((char*)buf) + amtRead;
      if (result == -1)
	{
	  cerr<< "Could not write.";
	  close(fd);
	  return -1;
	}
      if (result == 0)
	{
	  cerr<<"Connection died unexpectedly";
	  close(fd);
	  return 0;
	}
      amt -= result;
      amtRead += result;
    }
  return result;
}

ssize_t safeRead(int fd, void *buf, size_t count)
{
  size_t amt = count;
  size_t amtRead = 0;
  char* charCast = NULL;
  ssize_t result = read(fd, charCast, amt);
  while(amt)
    {
      charCast = ((char*)buf) + amtRead;
      if (result == -1)
	{
	  cerr<< "Could not write.";
	  close(fd);
	  return -1;
	}
      if (result == 0)
	{
	  cerr<<"Connection died unexpectedly";
	  close(fd);
	  return 0;
	}
      amt -= result;
      amtRead += result;
    }
  return result;
}
