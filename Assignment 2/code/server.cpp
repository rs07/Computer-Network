#include<iostream>
#include<bits/stdc++.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

int main()
{
	int server,client;
	int buffersize=1024;
	char buffer[buffersize];
	int portNum=80;
	
	struct sockaddr_in server_addr;
	socklen_t size;
	client=socket(AF_INET,SOCK_STREAM,0);
	if(client<0)
	{
		cout<<"Error establishing connection "<<endl;
		return 0;
	}
	cout<<"Socket connection established \n";
	server_addr.sin_family= AF_INET;
	server_addr.sin_addr.s_addr= htons(INADDR_ANY);
	server_addr.sin_port= htons(portNum);
	if(bind(client, (struct sockaddr *) &server_addr, sizeof(server_addr))<0)
	{
		cout<<"Error binding socket "<<endl;
		return 0;
	}
	size= sizeof(server_addr);
	cout<<"Looking for clients"<<endl;
	listen(client,1);
	server = accept(client, (struct sockaddr *)&server_addr, &size);
	if(server<0)
	{
		cout<<"Error on accepting"<<endl;
		return 0;
	}
	if(server>0)
	{
		do
		{
			int p= recv(server,buffer,buffersize,0);
			if(p>0)
			{
				cout<<"Client: ";
				printf("%s",buffer);
				cout<<"\n";
			}
			cout<<"Server: ";
			gets(buffer);
			send(server,buffer,buffersize,0);
			//cout<<"Server: ";
			//printf("%s\n",buffer);
		}while(strcmp(buffer,"#")!=0);
	}
	cout<<"Connection terminated ";
	//close(client);
}
