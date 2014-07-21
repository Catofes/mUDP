#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <stdlib.h>
#include <pthread.h>
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/IPAddress.h"
#include "Poco/Exception.h" 
#include "server.h"
using namespace std;
using namespace Poco;
using namespace Poco::Net;

//#define DEBUG

typedef void* (*FUNC)(void*);

int N=10;
ReceiveDatagramSocket *listener;
SocketAddress connectAddress("127.0.0.1",10085);

void SendDatagramSocket::run()
{
	while(true){
		int n=receiveBytes(this->localbuffer,sizeof(this->localbuffer)-1,0);
		if(n>0&&n<2048){
#ifdef DEBUG
			cout<<this->remoteAddress->toString()<<" -> "<<this->receiveAddress->toString()<<endl;			
#endif
			if(rand()%((n-50)>0?n-50:1)<50)
			  if(rand()%100<30){
				  this->sendid=rand()%N;
			  }else{
				  rand()%N;
			  }
			listener[this->sendid].sendTo(this->localbuffer, n, *(this->receiveAddress));
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

void SendDatagramSocket::send(SocketAddress *sender)
{
	if(this->onSend==1)return;
	this->onSend=1;
	int maxloop=0;
	if(this->waitTime>24){
	  while(this->buffers[this->packageId][0]<0&&maxloop<32)
		this->packageId=(this->packageId+1)%128;
	  this->waitTime=0;
	  cout<<"呵呵"<<endl;
	}
	maxloop=0;
	while(this->buffers[this->packageId][0]>=0&&maxloop<32){
		int n;
		memcpy(&n,this->buffers[this->packageId]+1,sizeof(int));
		cout<<this->buffers[this->packageId][0];
		this->sendBytes(this->buffers[this->packageId]+5,n);
#ifdef DEBUG
			cout<<sender->toString()<<" -> "<<this->remoteAddress->toString()<<this->buffers[this->packageId][0]<<endl;
#endif
		this->buffers[this->packageId][0]=-1;
		this->packageId=(this->packageId+1)%128;
		this->waitTime--;
		maxloop++;
	}
	this->onSend=0;
	return;
}

void Sender::send(char * buffer,int n, SocketAddress *sender)
{
	for(int i=this->sendSockets.size()-1;i>=0;i--){
		if(sender->toString()==this->sendSockets[i]->receiveAddress->toString()){
			char id=buffer[0]%128;
			//if(id!=(this->sendSockets[i]->packageId))cout<<"*";
			memcpy(this->sendSockets[i]->buffers[id]+1,buffer+1,n*sizeof(char));
			this->sendSockets[i]->buffers[id][0]=buffer[0];
			this->sendSockets[i]->waitTime++;
			this->sendSockets[i]->send(sender);
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
	newsocket->receiveAddressString=newsocket->receiveAddress->toString();
	newsocket->remoteAddress=new SocketAddress(connectAddress);
	newsocket->connect(*(newsocket->remoteAddress));
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

int main(int argc, char **argv)
{
	if (argc < 5){
		cout<<"Please Input STARTPORT ENDPORT REMOTEIPADDR REMOTEPORT."<<endl;
		return 0;
	}
	int startport=atoi(argv[1]);
	N=atoi(argv[2])-startport+1;
	if(N<=0){
		cout<<"Error Input"<<endl;
		return 0;
	}
	string remoteipaddr=argv[3];
	int port=atoi(argv[4]);
	SocketAddress send(remoteipaddr,port);
	connectAddress=send;
	ReceiveDatagramSocket temp[N];
	listener=new ReceiveDatagramSocket[N];
	Sender sender;
	for(int i=0;i<N;i++){
		listener[i].init("0.0.0.0",startport+i, &sender);
		listener[i].startThread();
	}
	pthread_exit(NULL);
}

