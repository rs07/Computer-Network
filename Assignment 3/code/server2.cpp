#include<iostream>
#include<bits/stdc++.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
using namespace std;


struct sockaddr_in server_addr;
struct sockaddr_storage serverStorage;
socklen_t size;

int main(int argc, char *argv[])
{
	int server,client;
	int buffersize=1024;
	char buffer[buffersize];
	int portNum=atoi(argv[1]);
	
	client=socket(AF_INET,SOCK_DGRAM,0);
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
	//recvfrom(client,buffer,buffersize,0,(struct sockaddr *)&serverStorage,&size);
	//cout<<"Client: ";
	//printf("%s\n",buffer);
	char msg[1000];
	while(1)
	{
		fflush(stdin);
		recvfrom(client,buffer,buffersize,0,(struct sockaddr *)&serverStorage,&size);
		//cout<<"Client: ";
		//printf("%s\n",buffer);
		char seq[2];
		seq[0]= buffer[0];
		seq[1]= buffer[1];
		printf("%02X %02X\n",seq[0],seq[1]);
		for(int i=7; i<=17; i++)
		{
			msg[i-7]= buffer[i];
		}
		printf("Message: %s\n",msg);
		printf("\n");
		char time_st[4];
		time_st[0]= buffer[2];
		time_st[1]= buffer[3];
		time_st[2]= buffer[4];
		time_st[3]= buffer[5];
		buffer[6]--;
		//cout<<"Server: ";
		//printf("%s\n",buffer);
		sendto(client,buffer,buffersize,0,(struct sockaddr *)&serverStorage, size);
		if(strcmp(buffer,"quit")==0)
			break;
	}
	cout<<"Connection terminated ";
	//close(client);
}
