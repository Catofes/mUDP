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
		char sendPackageId;
		char buffers[128][2161];
		bool onSend;
		char receivePackageId;
		char waitTime; 
		SendDatagramSocket()
		{
			for(int i=0;i<128;i++)
			  buffers[i][0]=-1;
			sendPackageId=0;
			receivePackageId=0;
			onSend=0;
			waitTime=0;
			sendPortNum=0;
			remoteAddress=new SocketAddress;
			receiveAddress=new SocketAddress;
		}
		void run();
		void send();
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

