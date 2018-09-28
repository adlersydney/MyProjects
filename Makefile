make all: Server Client

Server:	Server.cpp Hash.o
	g++ -Wall -Werror -g -lssl -lcrypto -o Server Server.o Hash.o

Server.o: Server.cpp
	g++ -Wall -Werror -g -c Server.cpp

Client: Client.o Hash.o
	g++ -Wall -Werror -lssl -lcrypto -g -o Client Client.o Hash.o

Client.o: Client.cpp
	g++ -Wall -Werror -g -c Client.cpp

Hash.o:	Hash.h	Hash.cpp
	g++ -Wall -Werror -g -c -lssl -lcrypto Hash.cpp

clean:
	rm *.o

