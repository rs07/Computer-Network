#include<iostream>
#include<bits/stdc++.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;
typedef long long ll;
#define IP "127.0.0.1"
int main()
{
	int port=80;
	int buffersize= 1024;
	char buffer[buffersize];
	//bool isExit;
	int client,server;

	sockaddr_in server_addr;
	client=socket(AF_INET,SOCK_STREAM,0);
	if(client<0)
	{
		cout<<"Error creating socket "<<endl;
		exit(1);
	}
	

	cout<<"Client Socket Created ";
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr= inet_addr(IP);
	server_addr.sin_port=htons(port);
	if(connect(client,(struct sockaddr *) &server_addr, sizeof(server_addr))==0)
	{
		cout<<"=> Connection to the server "<<inet_ntoa(server_addr.sin_addr)<<" with port number : "<<port<<endl;
		
		do
		{
			cout<<"Client: ";
			gets(buffer);
			int p=send(client,buffer,buffersize,0);
			cout<<"Server: ";
			recv(client,buffer,buffersize,0);
			printf("%s\n",buffer);
		}while(strcmp(buffer,"#")!=0);
		
		//cout<<"Connection Confirmed "<<"\n";
	}
	else
		cout<<"Connection Failed";
	cout<<"Connection terminated \n";
	//close(client);
}
