#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/IPAddress.h"
#include <iostream>

int main(int argc, char **argv)
{
	Poco::Net::SocketAddress socketAddress("0.0.0.0",10000);
	Poco::Net::DatagramSocket datagramSocket(socketAddress);

	char buffer[1024];
	int count=0;
	while (true)
	{
		count++;
		Poco::Net::SocketAddress sender;
		int n = datagramSocket.receiveFrom(buffer, sizeof(buffer) - 1, sender);
		buffer[n] = '\0';
		//if(count%1000000==0)
		std::cout << sender.toString() << ": " << buffer << std::endl;
		datagramSocket.sendTo(buffer,n, sender);
	}

	return 0;
}
