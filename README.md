mUDP
=========

###How to use

####Server

Edit server.cpp `SocketAddress connectAddress("222.29.157.251",10085);` Change Address to your server app. Like openvpn's 1194 port.

Edit server.cpp	`listener[i].init("0.0.0.0",8880+i, &sender);` Change port and ip that you want listen. This example will listen 8880-8889 ports

make and run

####Client

Edit client.cpp `SocketAddress connectAddress("127.0.0.1",8880);`  Change port and ip to server above .  This example either Port 8880 to 8889 is available.

Edit client.cpp `listener.init("0.0.0.0",8000, &sender);` Change ip and port that the program will listen to .  This example the user's openvpn client need to connect to port 8000 to get connect to Openvpn's Server 1194 above.
