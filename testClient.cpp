#include <iostream>
#include "Poco/Net/DatagramSocket.h"  
#include "Poco/Net/SocketAddress.h"  
#include "Poco/Timestamp.h"  
#include "Poco/DateTimeFormatter.h"  
#include <string>

int main()
{
	const char* ipaddr = "127.0.0.1";
	Poco::Net::SocketAddress sa("127.0.0.1", 5000);  
	Poco::Net::DatagramSocket dgs;
	//dgs.connect(sa); 

	std::string syslogMsg;  
	Poco::Timestamp now;  
	//syslogMsg = Poco::DateTimeFormatter::format(now, "<14>%w %f %H:%M:%S Hello,world!");
	syslogMsg="123456";
	//for(int i=0;i<1000;i++)
	  //syslogMsg+="t";
	int count=0;
	char buffer[2156];
	while(true){
		count++;
		if(count%1000==0)std::cout<<count<<std::endl;
		dgs.sendTo(syslogMsg.data(), syslogMsg.size(),sa);  
		int n = dgs.receiveFrom(buffer, sizeof(buffer) - 1,sa);
		buffer[n] = '\0';
		std::cout << sa.toString() << ": " << buffer << std::endl;
		break;
	}
	return 0;  
}  
