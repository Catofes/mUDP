mUDP
=========

This program split one udp socket to amount udps. In order to obfuscate the data.

You Need LIBPOCO to Make and Run.

###How to use

####Server

make 

Use ./server.out STARTPORT ENDPORT REMOTEIPADDR REMOTEPORT to run.

####Client

make

Use ./client.out IPADDR STARTPORT ENDPORT LOCALPORT to run.

####Example

	Server A:	ip 1.2.3.4 		Openvpn server listen on udp:8888
	
	Server B:	ip 1.2.3.5		./server.out 10000 10010 1.2.3.4 8888
	
	Client B:	ip 8.8.8.8		./client.out 1.2.3.5 10000 10010 9999
	
	Client A:	ip 8.8.8.9		Openvpn client connect to 8.8.8.8 9999

OR

	Server :	ip 1.2.3.4		Openvpn server listen on udp:127.0.0.1:8888
								./server.out 10000 10010 127.0.0.1 8888
	
	Client :	ip 8.8.8.8		./client.out 1.2.3.4 10000 10010 9999
								Openvpn client connect to 127.0.0.1 9999
