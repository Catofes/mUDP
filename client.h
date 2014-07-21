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
		int sendPortNum;
		unsigned char packageId;
		SendDatagramSocket()
		{
			packageId=0;
			sendPortNum=0;
			remoteAddress=new SocketAddress;
			receiveAddress=new SocketAddress;
		}
		void run();
		void startThread();
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

