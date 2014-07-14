#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <stdlib.h>
#include <pthread.h>
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/IPAddress.h"
#include "Poco/Exception.h" 
#include "dUDPe.h"
using namespace std;
using namespace Poco;
using namespace Poco::Net;

int const N=10;
Listener listener[N];
SocketAddress remoteAddress("222.29.157.251",10085);
int Sender::AddSocket(SocketAddress * sender)
{
	MyDatagrameSocket *newsocket=new MyDatagrameSocket;
	newsocket->sender= * sender;
	newsocket->connect(remoteAddress);
	newsocket->gthread();
	remoteSocket.push_back(newsocket);
};

void MyDatagrameSocket::gthread()
{
	typedef void* (*FUNC)(void*);//定义FUNC类型是一个指向函数的指针，该函数参数为void*，返回值为void*  
	FUNC callback = (FUNC)&MyDatagrameSocket::run;//强制转换func()的类型  
	int ret=pthread_create(&tids, NULL,callback , this);
};

void MyDatagrameSocket::run()
{
	while(true){
		int n=0;
		n=receiveBytes(this->localbuffer, sizeof(this->localbuffer)-1,0);
		if(n>0&&n<2047){
			this->count++;
			int randomnum=this->lastid;
			if(this->count%10==2)randomnum=rand()%N;
			listener[randomnum].localSocket->sendTo(localbuffer, n, sender);
		}
	}
};

int Sender::send(char *buffer, int n, SocketAddress *sender,SocketAddress * receiver,int id)
{
	for(int i=remoteSocket.size()-1;i>=0;i--){
		if(sender->toString()==remoteSocket[i]->sender.toString())
		{
			remoteSocket[i]->sendTo(buffer, n,remoteAddress);
			remoteSocket[i]->lastid=id;
			return 0;
		}
	}
	AddSocket(sender);
	send(buffer,n,sender,receiver,id);
}
void Listener::run()
{
	while(true){
		int n=0;
		n=localSocket->receiveFrom(this->localbuffer, sizeof(this->localbuffer)-1,*sender);
		if(n>0&&n<2047){
			remote->send(localbuffer,n,sender,localSocketAddress,this->id);
		}
	}
};
void Listener::gthread()
{
	typedef void* (*FUNC)(void*);//定义FUNC类型是一个指向函数的指针，该函数参数为void*，返回值为void*  
	FUNC callback = (FUNC)&Listener::run;//强制转换func()的类型  
	int ret=pthread_create(&tids, NULL, callback , this);
};


int main()
{
	Sender sender;
	for(int i=0;i<N;i++){
		listener[i].init("0.0.0.0",10080+i,&sender,i);
		listener[i].gthread();
	}
	pthread_exit(NULL);
}

/*
   int main(int argc, char **argv)
   {
   SocketAddress localSocketAddress("127.0.0.1",10999);
   DatagramSocket localSocket(localSocketAddress);
   SocketAddress localSocketAddress2("127.0.0.1",11000);
   DatagramSocket localSocket2(localSocketAddress2);
   cout<<localSocket.supportsIPv6()<<endl;
   SocketAddress remoteSocketAddress("222.29.157.251",10085);
   DatagramSocket remoteSocket;
   remoteSocket.connect(remoteSocketAddress);
   char localbuffer[2156];
   char remotebuffer[2156];
   SocketAddress sender;
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

*/
