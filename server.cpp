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
		int n=receiveBytes(this->localbuffer+5,sizeof(this->localbuffer)-5,0);
		if(n>0&&n<2048){
#ifdef DEBUG
			cout<<"SEND:"<<this->remoteAddress->toString()<<" -> "<<this->receiveAddress->toString()<<" Lenght: "<<n<<endl;			
#endif
			localbuffer[0]=this->sendPackageId;
			memcpy(localbuffer+1,&n,sizeof(int));
			if(rand()%((n-50)>0?n-50:1)<50)
			  if(rand()%100<30){
				  this->sendid=rand()%N;
			  }else{
				  rand()%N;
			  }
			listener[this->sendid].sendTo(this->localbuffer, n+5, *(this->receiveAddress));
			this->sendPackageId=(this->sendPackageId+1)%128;
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
	if(this->waitTime>6){
		while(this->buffers[this->receivePackageId][0]<0&&maxloop<32)
		  this->receivePackageId=(this->receivePackageId+1)%128;
		this->waitTime=0;
	}
	maxloop=0;
	while(this->buffers[this->receivePackageId][0]>=0&&maxloop<32){
		int n;
		memcpy(&n,this->buffers[this->receivePackageId]+1,sizeof(int));
//		cout<<this->buffers[this->receivePackageId][0];
		this->sendBytes(this->buffers[this->receivePackageId]+5,n);
#ifdef DEBUG
		cout<<"GET :"<<sender->toString()<<" -> "<<this->remoteAddress->toString()<<" Length: "<<n<<endl;
#endif
		this->buffers[this->receivePackageId][0]=-1;
		this->receivePackageId=(this->receivePackageId+1)%128;
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

