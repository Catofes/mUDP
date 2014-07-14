using namespace std;
using namespace Poco::Net;

class MyDatagrameSocket : public DatagramSocket
{
	public:
		pthread_t tids;
		int lastid;
		int count;
		char localbuffer[2156];
		SocketAddress sender;
		MyDatagrameSocket()
		{
			count=0;
		}
		MyDatagrameSocket(const MyDatagrameSocket & old) 
		{
			//DatagramSocket::DatagramSocket(old);
			sender=old.sender;
		}
		void run();
		void gthread();
};
class Sender
{
	private:
		vector<MyDatagrameSocket*> remoteSocket;
	public:
		int send(char * buffer,int n,SocketAddress *sender,SocketAddress *receiver,int id);
		int AddSocket(SocketAddress *sender);
};
class Listener 
{
	public:
		int id;
		SocketAddress *localSocketAddress;
		pthread_t tids;
		DatagramSocket *localSocket;
		char localbuffer[2156];
		SocketAddress *sender;
		Sender * remote;
		void init (string ipAddress, int Port,Sender *iremote,int id)
		{
			localSocketAddress=new SocketAddress(ipAddress.c_str(),Port);
			localSocket=new DatagramSocket();
			localSocket->bind(*localSocketAddress,true);
			sender=new SocketAddress();
			remote=iremote;
			this->id=id;
		}
		void run();
		void gthread();
};
