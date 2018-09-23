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
#include<pthread.h>
using namespace std;
typedef long long ll;
#define IP "10.31.5.75"


void *read_from_server(void *ptr)
{
	int client, p;
	client= *((int *)ptr);
	int buffersize=1024;
	char buffer[buffersize];
	while(1)
	{
		recv(client,buffer,buffersize,0);
		cout<<"Server: ";
		printf("%s\n",buffer);
		if(strcmp(buffer,"quit")==0)
			break;
	}
}

/*void *write_from_server(void *ptr)
{
	int *client,p;
	client= (int *)ptr;
	int buffersize=1024;
	char buffer[buffersize];
	while(1)
	{
		cin>>buffer;
		send(client,buffer,buffersize,0);
		cout<<"Client: ";
		printf("%s\n",buffer);
	}
}*/

int main()
{
	int port=1800;
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
		
		/*do
		{
			cout<<"Client: ";
			gets(buffer);
			int p= send(client,bufstrlen(fer,bu)size,0);
			cout<<"Server: ";		cout<<"Client: ";
			cout<<buffer
			recv(client,buffer,buffersize,0);
			printf("%s\n",buffer);
		}while(strcmp(buffer,"#")!=0);*/
		
		//cout<<"Connection Confirmed "<<"\n";
		pthread_t read_t, write_t;
		pthread_create( &read_t, NULL, read_from_server, &client);
		cout<<"Read Thread Created \n";
		/*pthread_create( &write_t, NULL, write_from_server, (void *)client);
		cout<<"Write Thread Created \n";*/
		while(1)
		{
			//
			gets(buffer);
			cout<<"Client: ";
			printf("%s\n",buffer);
			send(client,buffer,buffersize,0);
			if(strcmp(buffer,"quit")==0)
				break;
		}
		pthread_join(read_t, NULL);
    	//pthread_cancel(read_t);
	}
	else
		cout<<"Connection Failed";
	cout<<"Connection terminated \n";
	//close(client);
}
