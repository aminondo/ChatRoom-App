server: server.o users.o
	g++ -pthread -std=c++11 -o server server.o users.o

server.o: server.cpp
	g++ -c server.cpp

users.o: users.cpp
	g++ -c users.cpp

clean:
	rm *.o server
