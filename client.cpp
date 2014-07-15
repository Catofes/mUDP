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

