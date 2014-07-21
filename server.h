using namespace std;
using namespace Poco::Net;

class MyDatagramSocket : public DatagramSocket
{
	public:
		pthread_t tids;
		char localbuffer[2156];
		MyDatagramSocket(){};
};

class SendDatagramSocket : public MyDatagramSocket
{
	public:
		SocketAddress *remoteAddress;
		SocketAddress *receiveAddress;
		string receiveAddressString;
		int sendid;
		char buffers[64][2161];
		bool onSend;
		char packageId;
		char waitTime;
		SendDatagramSocket()
		{
			for(int i=0;i<64;i++)
			  buffers[i][0]=-1;
			sendid=0;
			packageId=0;
			waitTime=0;
			receiveAddressString="";
			remoteAddress=new SocketAddress;
			receiveAddress=new SocketAddress;
		}
		void run();
		void startThread();
		void send(SocketAddress *sender);
};

class Sender
{
	public:
		vector<SendDatagramSocket*> sendSockets;
		void send(char * buffer,int n, SocketAddress *sender);
		void AddSocket(SocketAddress * sender);
};

class ReceiveDatagramSocket : public MyDatagramSocket
{
	public:
		SocketAddress *listeningAddress;
		SocketAddress *receiveAddress;
		Sender * sender;
		void init(string ipAddress, int Port,Sender *isender);
		void run();
		void startThread();

};

