#define __USE_BSD  /* 使用BSD风格的结构定义 */ 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#define __FAVOR_BSD /* use bsd'ish tcp header */ 
#include <netinet/udp.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <map>
using namespace std;

#define DEBUG

int startport=9000;
int endport=9100;
struct sockaddr_in local;
struct sockaddr_in remote;
int local_startport=20000;
int local_endport=30000;
int local_port=20000;

struct pesudo_udphdr {
	unsigned int saddr, daddr;
	unsigned char unused;
	unsigned char protocol;
	unsigned short udplen;
};

struct info{
	pesudo_udphdr head;
	udphdr	port;
	sockaddr_in dst;
	int newport;
};

map<string,info> client_info;
map<int,info> server_info;

unsigned short in_cksum(unsigned short *addr, int len) 
{ 
	int sum=0; 
	unsigned short res=0; 
	while( len > 1)  {   
		sum += *addr++; 
		len -=2; 
	}   
	if( len == 1) { 
		*((unsigned char *)(&res))=*((unsigned char *)addr); 
		sum += res; 
	}   
	sum = (sum >>16) + (sum & 0xffff); 
	sum += (sum >>16) ; 
	res = ~sum; 
	return res; 
} 


void raw_send(char * buff, char * send_buff,int len,info sendinfo,int rawsock)
{
	struct pesudo_udphdr *pudph=(struct pesudo_udphdr*)send_buff;
	struct udphdr *udph=(struct udphdr*)(8+send_buff+sizeof(struct pesudo_udphdr));
	char *data=(char *)(udph+1);
	memcpy(data, buff,len-28);
	pudph->saddr=sendinfo.head.saddr;
	pudph->daddr=sendinfo.head.daddr;
	pudph->unused=0;
	pudph->protocol=IPPROTO_UDP;
	pudph->udplen=htons(len-20);

	udph->uh_sport=sendinfo.port.uh_sport;
	udph->uh_dport=sendinfo.port.uh_dport;
	udph->uh_ulen=htons(len-20);
	udph->uh_sum=0;
	
	/* include pesudo header and udp header*/
	udph->uh_sum=in_cksum((unsigned short*)pudph, len);
	int sendlen=sendto(rawsock, udph, len-12, 0,
				(struct sockaddr *)&(sendinfo.dst), sizeof(sendinfo.dst));
	
#ifdef DEBUG
	if( sendlen < 0)
	  perror("sendto() error");
	else{
	  printf("sendto() send %d bytes\n", len);
	
	printf("Send %s:%d to %s:%d len=%d  ip_len=%d\n", 
				"local", 
				ntohs(udph->uh_sport), 
				inet_ntoa(sendinfo.dst.sin_addr), 
				ntohs(sendinfo.dst.sin_port), 
				len, ntohs(pudph->udplen) 
		  );
	}
#endif
}


void dump(char *buff,char *send_buff,int len,int rawsock)
{
	struct ip *iph=(struct ip *)buff;
	int i=iph->ip_hl*4;
	struct udphdr *udph=(struct udphdr *)&buff[i];
	char *data=(char *)(udph+1);
	//first get src ip and port infomation
	string src_ip_port = inet_ntoa(iph->ip_src);
	src_ip_port=src_ip_port+":"+to_string(udph->uh_sport);
	//then judge if this connection is establish
	map<string,info>::iterator info_to_send;
	info_to_send=client_info.find(src_ip_port);
#ifdef DEBUG
	//cout<<"Data receive From :"<<src_ip_port<<endl;
#endif
	if(info_to_send==client_info.end()){
		//if client_info don't have any info
		if(ntohs(udph->uh_dport)<=endport&&ntohs(udph->uh_dport)>=startport&&iph->ip_dst.s_addr==local.sin_addr.s_addr){
			//this is a new connection from client
			//prepare head
			info newinfo;
			newinfo.head.saddr=local.sin_addr.s_addr;
			newinfo.head.daddr=remote.sin_addr.s_addr;
			newinfo.head.unused=0;
			newinfo.head.protocol=IPPROTO_UDP;

			//generate send port
			int newport=local_port;
			local_port++;
			if(local_port>local_endport)
			  local_port=local_startport;

			newinfo.port.uh_sport=htons(newport);
			newinfo.port.uh_dport=remote.sin_port;

			newinfo.dst.sin_addr=remote.sin_addr;
			newinfo.dst.sin_port=remote.sin_port;
			newinfo.dst.sin_family=AF_INET;
			
			newinfo.newport=newport;
			client_info.insert(std::pair<string,info>(src_ip_port,newinfo));

			//prepare server_info
			newinfo.head.saddr=iph->ip_dst.s_addr;
			newinfo.head.daddr=iph->ip_src.s_addr;
			newinfo.head.unused=0;
			newinfo.head.protocol=IPPROTO_UDP;

			//newinfo.port.uh_sport=htons(startport+rand()%(endport-startport+1));
			newinfo.port.uh_sport=udph->uh_dport;
			newinfo.port.uh_dport=udph->uh_sport;

			newinfo.dst.sin_addr=iph->ip_src;
			newinfo.dst.sin_port=udph->uh_sport;
			newinfo.dst.sin_family=AF_INET;

			server_info[newport]=newinfo;
#ifdef DEBUG
			cout<<"New Connection Establish, Use Port: "<<newport<<endl;
#endif
		}
	}
	//research
	info_to_send=client_info.find(src_ip_port);
	if(info_to_send!=client_info.end()){
#ifdef DEBUG
		printf("From %s:%d to %s:%d len=%d iphdr_len=%d ip_len=%d\n",
					inet_ntoa(iph->ip_src),
					ntohs(udph->uh_sport),
					inet_ntoa(iph->ip_dst),
					ntohs(udph->uh_dport),
					len, i, ntohs(iph->ip_len)
			  );
#endif
		server_info[info_to_send->second.newport].port.uh_sport=udph->uh_dport;
		raw_send(data,send_buff,len,info_to_send->second,rawsock);
		return;
	}
	map<int,info>::iterator info_to_get;
	info_to_get=server_info.find(ntohs(udph->uh_dport));
	if(info_to_get!=server_info.end()){
		//info_to_get->second.port.uh_sport=htons(startport+rand()%(endport-startport+1));
#ifdef DEBUG	
		printf("From %s:%d to %s:%d len=%d iphdr_len=%d ip_len=%d\n",
					inet_ntoa(iph->ip_src),
					ntohs(udph->uh_sport),
					inet_ntoa(iph->ip_dst),
					ntohs(udph->uh_dport),
					len, i, ntohs(iph->ip_len)
			  );
#endif
		raw_send(data,send_buff,len,info_to_get->second,rawsock);
		return;
	}
	return;
	cout<<"ERROR"<<endl;
	printf("From %s:%d to %s:%d len=%d iphdr_len=%d ip_len=%d\n",
				inet_ntoa(iph->ip_src),
				ntohs(udph->uh_sport),
				inet_ntoa(iph->ip_dst),
				ntohs(udph->uh_dport),
				len, i, ntohs(iph->ip_len)
		  );

}

int main(int argc, char *argv[])
{
	int rawsock,len;
	char buff[8196];
	char sendbuf[8192];
	if(argc < 5) {
		printf("usage: %s localip startport endport remoteip remoteport (local_startport local_remoteport) \n",argv[0]);
		exit(1);
	}
	if( inet_aton(argv[1], &local.sin_addr) == 0) {
		printf("bad localip:%s\n", argv[1]);
		exit(1);
	}
	if( inet_aton(argv[4], &remote.sin_addr) == 0) {
		printf("bad remoteip:%s\n", argv[4]);
		exit(1);
	}
	local.sin_port=htons(atoi(argv[2]));
	startport=atoi(argv[2]);
	endport=atoi(argv[3]);
	remote.sin_port=htons(atoi(argv[5]));
	local.sin_family=AF_INET;
	remote.sin_family=AF_INET;

	rawsock=socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
	if( rawsock < 0) {
		perror("socket");
		exit(1);
	}
	while( (len=read(rawsock, buff, 8192)) > 0) {
		dump(buff,sendbuf,len,rawsock);
	}
	close(rawsock);
	exit(0);
}

