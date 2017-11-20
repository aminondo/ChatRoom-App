server: server.o users.o
	g++ -o server server.o users.o

server.o: server.cpp
	g++ -c server.cpp

users.o: users.cpp
	g++ -c users.cpp

clean:
	rm *.o server
