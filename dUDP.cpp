#include <iostream>
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/IPAddress.h"

using namespace std;

int main(int argc, char **argv)
{
	Poco::Net::SocketAddress localSocketAddress("127.0.0.1",10999);
	Poco::Net::DatagramSocket localSocket(localSocketAddress);
	Poco::Net::SocketAddress localSocketAddress2("127.0.0.1",11000);
	Poco::Net::DatagramSocket localSocket2(localSocketAddress2);
	cout<<localSocket.supportsIPv6()<<endl;
	Poco::Net::SocketAddress remoteSocketAddress("222.29.157.251",10085);
	Poco::Net::DatagramSocket remoteSocket;
	remoteSocket.connect(remoteSocketAddress);
	char localbuffer[2156];
	char remotebuffer[2156];
	Poco::Net::SocketAddress sender;
	int k = localSocket.receiveFrom(localbuffer , sizeof(localbuffer)-1 , sender );
	if( k != 0){
		localbuffer[k]='\0';
		std::cout << sender.toString() << ": " << localbuffer << std::endl;
		remoteSocket.sendBytes( localbuffer, k);
	}
	


	while(true)
	{
		int n,m;
		n = localSocket.receiveBytes(localbuffer , sizeof(localbuffer)-1 );
		if( n != 0){
			localbuffer[n]='\0';
			std::cout << sender.toString() << ": " << std::endl;
			for(int j=0;j<n;j++)
			  cout<<hex<<(((int)localbuffer[j])&255)<<"|";
			cout<<endl;
			remoteSocket.sendBytes( localbuffer, n);
		}
		m = remoteSocket.receiveBytes(remotebuffer , sizeof(remotebuffer)-1);
		if( m != 0){
			remotebuffer[m]='\0';
			std::cout << "222.29.157.251 : "<< std::endl;
			for(int j=0;j<m;j++)
			  cout<<hex<<(((int)remotebuffer[j])&255)<<"|";
			cout<<endl;
			localSocket2.sendTo( remotebuffer , m , sender );
		}
		n = localSocket2.receiveBytes(localbuffer , sizeof(localbuffer)-1 );
		if( n != 0){
			localbuffer[n]='\0';
			std::cout << sender.toString() << ": " << std::endl;
			for(int j=0;j<n;j++)
			  cout<<hex<<(((int)localbuffer[j])&255)<<"|";
			cout<<endl;
			remoteSocket.sendBytes( localbuffer, n);
		}
		m = remoteSocket.receiveBytes(remotebuffer , sizeof(remotebuffer)-1);
		if( m != 0){
			remotebuffer[m]='\0';
			std::cout << "222.29.157.251 : "<< std::endl;
			for(int j=0;j<m;j++)
			  cout<<hex<<(((int)remotebuffer[j])&255)<<"|";
			cout<<endl;
			localSocket.sendTo( remotebuffer , m , sender );
		}
	}
}	
