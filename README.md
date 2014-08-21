mUDP
=========

This program split one udp socket to amount udps. In order to obfuscate the data.

You Need LIBPOCO to Make and Run.

###How to use

####Server

make 

Use ./server.out LOCALIP STARTPORT ENDPORT REMOTEIPADDR REMOTEPORT to run.

Due to RAW_SOCKET. You need to block icmp package which type 3 (Destination Unreachable). You can use iptables to do this.

sudo iptables -A OUTPUT -p icmp --icmp-type 3 -j DROP

####Client

make

Use ./client.out IPADDR STARTPORT ENDPORT LOCALPORT to run.

The client must NOT under SNAT.

####Example

	Server A:	ip 1.2.3.4 		Openvpn server listen on udp:8888
	
	Server B:	ip 1.2.3.5		./server.out 1.2.3.4 10000 10010 1.2.3.4 8888
	
	Client B:	ip 8.8.8.8		./client.out 1.2.3.5 10000 10010 9999
	
	Client A:	ip 8.8.8.9		Openvpn client connect to 8.8.8.8 9999

OR

	Server :	ip 1.2.3.4		Openvpn server listen on udp:1.2.3.4:8888
								./server.out 1.2.3.4 10000 10010 1.2.3.4 8888
	
	Client :	ip 8.8.8.8		./client.out 1.2.3.4 10000 10010 9999
								Openvpn client connect to 127.0.0.1 9999

####To Do:

* There still have data out of order . So add package id in it and sort the package in the server.
* Rewrite the client part. In order to solve the SNAT program.
