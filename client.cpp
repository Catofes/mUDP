#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <stdlib.h>
#include <pthread.h>
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/IPAddress.h"
#include "Poco/Exception.h" 
#include "client.h"
using namespace std;
using namespace Poco;
using namespace Poco::Net;

//#define DEBUG

typedef void* (*FUNC)(void*);

ReceiveDatagramSocket listener;
SocketAddress connectAddress("127.0.0.1",8880);

void SendDatagramSocket::run()
{
	while(true){
		int n=receiveFrom(this->localbuffer,sizeof(this->localbuffer)-1,*(this->remoteAddress));
		if(n>0&&n<2048){
#ifdef DEBUG
			cout<<this->remoteAddress->toString()<<" -> "<<this->receiveAddress->toString()<<endl;			
#endif
			listener.sendTo(this->localbuffer, n, *(this->receiveAddress));
		}
	}
}

void SendDatagramSocket::startThread()
{
	FUNC callback = (FUNC)&SendDatagramSocket::run;
	pthread_create(&(this->tids),NULL,callback,this);
#ifdef DEBUG
	cout<<"Create new thread at :"<<this->tids<<endl;
#endif
}

void Sender::send(char * buffer,int n, SocketAddress sender)
{
	for(int i=this->sendSockets.size()-1;i>=0;i--){
		if(sender.toString()==this->sendSockets[i]->receiveAddress->toString()){
			sendSockets[i]->sendTo(buffer,n,*(sendSockets[i]->remoteAddress));
#ifdef DEBUG
			cout<<sender->toString()<<" -> "<<sendSockets[i]->remoteAddress->toString()<<endl;
#endif
			return ;
		}
	}
	AddSocket(&sender);
	send(buffer,n,sender);
}

void Sender::AddSocket(SocketAddress *sender)
{
	SendDatagramSocket *newsocket=new SendDatagramSocket;
	newsocket->receiveAddress=new SocketAddress(*sender);
	newsocket->remoteAddress=new SocketAddress(connectAddress);
	//newsocket->connect(connectAddress);
	newsocket->startThread();
	this->sendSockets.push_back(newsocket);
}

void ReceiveDatagramSocket::init(string ipAddress, int Port,Sender *isender)
{
	this->listeningAddress=new SocketAddress(ipAddress,Port);
	this->receiveAddress=new SocketAddress();
	this->bind(*(this->listeningAddress));
	this->sender=isender;
}

void ReceiveDatagramSocket::run()
{
	while(true){
		int n=this->receiveFrom(this->localbuffer, sizeof(this->localbuffer)-1, *(this->receiveAddress));
		if(n>0&&n<2048)
			sender->send(this->localbuffer, n,*(this->receiveAddress));
	}
}

void ReceiveDatagramSocket::startThread()
{
	FUNC callback = (FUNC)&ReceiveDatagramSocket::run;
	pthread_create(&(this->tids),NULL,callback,this);
	cout<<"Start Listening at: "<<this->listeningAddress->toString()<<endl;
}

int main(int argc,char **argv)
{
	Sender sender;
	listener.init("0.0.0.0",8000, &sender);
	listener.startThread();
	pthread_exit(NULL);
}

/*
void Listener::run()
{
	while(true){
		int n=0;
		n=localSocket->receiveFrom(this->localbuffer, sizeof(this->localbuffer)-1,*sender);
		cout<<"aaaaaaaaaaaaaaaaaaaaa:	"<<n<<endl;
		if(n>0&&n<2047){
			std::cout <<"receive from local :"<< sender->toString() << " -> "<<localSocketAddress->toString()<<" :" << std::endl;
			for(int j=0;j<n;j++)
			  cout<<hex<<(((int)localbuffer[j])&255)<<"|";
			cout<<endl;
			remote->send(localbuffer,n,sender,localSocketAddress,this->id);
		}
	}
};
void Listener::gthread()
{
	typedef void* (*FUNC)(void*);//定义FUNC类型是一个指向函数的指针，该函数参数为void*，返回值为void*  
	FUNC callback = (FUNC)&Listener::run;//强制转换func()的类型  
	int ret=pthread_create(&tids, NULL, callback , this);
	cout<<"create Thread:"<<ret<<endl;
};


int main()
{
	Sender sender;
	for(int i=0;i<N;i++){
		listener[i].init("0.0.0.0",10080+i,&sender,i);
		listener[i].gthread();
	}
	pthread_exit(NULL);
	cout<<"hehe"<<endl;
}

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
