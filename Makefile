all:mUDP

mUDP:
	g++ client.cpp -o client.out -lPocoNet -lpthread -lPocoFoundation
	g++ server.cpp -o server.out -lPocoNet -lpthread -lPocoFoundation
debug:
	g++ -g client.cpp -o client.out -lPocoNet -lpthread -lPocoFoundation
	g++ -g server.cpp -o server.out -lPocoNet -lpthread -lPocoFoundation
test:
	g++ -g testClient.cpp -o testClient.out -lPocoNet -lpthread -lPocoFoundation
	g++ -g testServer.cpp -o testServer.out -lPocoNet -lpthread -lPocoFoundation
clean:
	client.out
	server.out
