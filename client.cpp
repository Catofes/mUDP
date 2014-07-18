#include <iostream>
#include <cstring>
#include <string>
#include <cstring>
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
string Host="128.199.152.20";
int StartPort=8880;
int EndPort=8889;
SocketAddress *connectAddress;

void SendDatagramSocket::run()
{
	while(true){
		int n=receiveFrom(this->localbuffer,sizeof(this->localbuffer)-1,*(this->remoteAddress));
		if(n>0&&n<2048){
#ifdef DEBUG
			cout<<"GET :"<<this->remoteAddress->toString()<<" -> "<<this->receiveAddress->toString()<<endl;			
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

void Sender::send(char * buffer,int n, SocketAddress *sender)
{
	for(int i=this->sendSockets.size()-1;i>=0;i--){
		if(sender->toString()==this->sendSockets[i]->receiveAddress->toString()){
			if(rand()%n-100<0)
			  if(rand()%100<30){
				  this->sendSockets[i]->sendPortNum=StartPort+rand()%(EndPort-StartPort+1);
			  }else{
				  StartPort+rand()%(EndPort-StartPort+1);
			  }
			  else
				rand()%100;
			SocketAddress SendToAddress(Host,this->sendSockets[i]->sendPortNum);
			sendSockets[i]->sendTo(buffer,n,SendToAddress);
#ifdef DEBUG
			cout<<"SEND:"<<sender->toString()<<" -> "<<SendToAddress.toString()<<endl;
#endif
			return ;
		}
	}
	AddSocket(sender);
	send(buffer,n,sender);
}

void Sender::AddSocket(SocketAddress *sender)
{
	SendDatagramSocket *newsocket=new SendDatagramSocket;
	newsocket->receiveAddress=new SocketAddress(*sender);
	newsocket->remoteAddress=new SocketAddress(*connectAddress);
	newsocket->sendPortNum=StartPort+rand()%(EndPort-StartPort+1);
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
		  sender->send(this->localbuffer, n,this->receiveAddress);
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
	if (argc < 5)
	{   
		cout<<" Please Input As REMOTEIPADDRESS STARTPORT ENDPORT LOCALPORT."<<endl;
		return 0;
	} 
	string IPADDRESS=argv[1];
	Host=IPADDRESS;
	StartPort=atoi(argv[2]);
	EndPort=atoi(argv[3]);
	connectAddress=new SocketAddress(Host,StartPort);

	Sender sender;
	listener.init("0.0.0.0",atoi(argv[4]), &sender);
	listener.startThread();
	pthread_exit(NULL);
}

