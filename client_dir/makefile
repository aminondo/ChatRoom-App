client: client.o
	g++ -pthread -std=c++11 -o client client.o

client.o: client.cpp
	g++ -c client.cpp

clean:
	rm *.o client
