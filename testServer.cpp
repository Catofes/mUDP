#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/IPAddress.h"
#include <iostream>

int main(int argc, char **argv)
{
	Poco::Net::SocketAddress socketAddress(Poco::Net::IPAddress(), 10085);
	Poco::Net::DatagramSocket datagramSocket(socketAddress);

	char buffer[1024];
	int count=0;
	while (true)
	{
		count++;
		Poco::Net::SocketAddress sender;
		int n = datagramSocket.receiveFrom(buffer, sizeof(buffer) - 1, sender);
		buffer[n] = '\0';
		std::cout << sender.toString() << ": " << buffer << std::endl;
	}

	return 0;
}
