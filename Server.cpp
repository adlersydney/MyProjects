/*
  CSE109: Summer 2018
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
  int mySocket;
  if((mySocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("Socket failed");
      exit(1);
    }
  
  sockaddr_in server;
  memset(&server, 0, sizeof(sockaddr_in));
 
 server.sin_family = AF_INET;
 server.sin_addr.s_addr = htonl(INADDR_ANY);
 
 bool retry = true;
 int bound = 0;
 short portNum = 10000;
 while(retry)
   {
     if(portNum >= 10500)
       {
	 perror("Couldn't find a port after 500 tries");
	 exit(2);
       }

     server.sin_port = htons(portNum);
     bound = bind(mySocket, (sockaddr*)&server, sizeof(sockaddr_in));
     if(bound < 0)
       {
	 if(errno == EBADF)
	   {
	     perror("Socket is invalid");
	     exit(3);
	   }
	 if(errno == EINVAL)
	   {
	     perror("Socket already bound");
	     exit(4);
	   }
	 portNum++;
	 continue;
       }

     const int QUEUESIZE = 10;
     linger linger_option = {1, QUEUESIZE};
     setsockopt(mySocket, SOL_SOCKET, SO_LINGER, &linger_option, sizeof(linger));
     if(listen(mySocket, QUEUESIZE) < 0)
       {
	 portNum++;
	 continue;
       }
     retry = false;
     
   }
 
 cerr << "Listening from port: " << portNum << endl;

 char myFile[]  = "connection.txt";
 int fd = open(myFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd == -1)
   {
     cerr << "File open failed.";
     exit(7);
   }
  cer << "Connection.txt created." << endl;
  
 write(fd, &portNum, 2);
 char host[1024];
 gethostname(host,1023);
 char* hostName = host;
 int hostSize = (int)strlen(hostName);
 write(fd, &hostSize, 4);
 write(fd, hostName, strlen(hostName));
 close(fd);

 char* served = argv[1];
 unsigned short request = 1;
 while (request > 0)
   {
     sockaddr_in other;
     memset(&other, 0, sizeof(sockaddr_in));
     socklen_t acceptedLen;
     memset(&acceptedLen, 0, sizeof(socklen_t));
     acceptedLen = sizeof(sockaddr_in);

    int  accepted = accept(mySocket, (sockaddr*)&other, &acceptedLen);
     if(accepted == -1)
       {
	 perror("Accept failed");
	 exit(4);
       }

     safeRead(accepted, &acceptedLen, 2);
     if(acceptedLen < 10 || acceptedLen > 65535)
       {
	 cerr << "Client request out of bounds size.";
	 exit(5);
       }
     int amount;
     int start;
     safeRead(accepted,&start, 4);
     safeRead(accepted,&amount, 4);
     
     int servedfd = open(served, O_RDONLY);
     
     if(servedfd == -1)
       {
	 cerr << "Served file open failed.";
       }
     off_t seek = lseek(servedfd, start, SEEK_SET);
     if(seek == -1)
       {
	 cerr << "Seek from request failed";
       }
     char* buff = (char*)malloc(amount);
     int amountRead = safeRead(servedfd, buff, amount);
     unsigned char* hashed = Hash(buff, amountRead);
     int dataSentSize = amount + (int)strlen((const char*)hashed);
     safeWrite(accepted, &dataSentSize, 4);
     safeWrite(accepted, &amount, 4);
     safeWrite(accepted, buff, strlen(buff));
     safeWrite(accepted, hashed, strlen((const char*)hashed));

       if (close(accepted))
       {
	 cerr << "Closing the acceptedFD error (" << errno << "): " << strerror(errno);
	 exit(5);
       }

     free(buff);
     buff = NULL;
   }
 return 0;
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
