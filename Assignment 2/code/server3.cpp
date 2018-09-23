#include<iostream>
#include<bits/stdc++.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<pthread.h>
#include<unistd.h>
using namespace std;
#define MAX 10
int c=0;

int server[MAX],client;

/*void *write_from_client(void *ptr)
{
	int *client,p;
	client= (int *)ptr;
	int buffersize=1024;
	char buffer[buffersize];
	while(1)
	{
		gets(buffer);
		send(client,buffer,buffersize,0);
		cout<<"Server: ";
		printf("%s\n",buffer);
		cout<<"\n";
	}
}
*/
void send_to_all(char *buff, int buffersize, int ser)
{
	for(int i=0; i<c; i++)
	{
		if(server[i]!=ser)
		{
			send(server[i],buff,buffersize,0);
		}
	}
}


void quiting_server(int ser)
{
	if(c==0)
	{
		memset(server,-1,sizeof(server));
		c--;
		close(ser);
		return;
	}
	for(int i=0; i<c; i++)
	{
		if(server[i]==ser)
		{
			close(ser);
			while(i!=c-1)
			{
				server[i]=server[i+1];
				i++;
			}
			c--;
			break;
		}
	}
}

void *read_from_client(void *ptr)
{
	int ser,p;
	ser= *((int *)ptr);
	int buffersize=1024;
	char buffer[buffersize];
	string msg;
	int z=0;
	while(1)
	{
		int p= recv(ser,buffer,buffersize,0);
		if(p<0)
		{
			quiting_server(ser);
			break;
		}
		send_to_all(buffer, buffersize, ser);
		//cout<<"Client: ";
		printf("%s\n",buffer);
		z++;
		if(strcmp(buffer,"quit")==0)
		{
			quiting_server(ser);
			break;
		}
		if(z>80)
			break;
	}
}

int main()
{
	
	int buffersize=1024;
	char buffer[buffersize];
	int portNum=1500,n;
	
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
	cout<<"Enter the number of clients will join: ";
	cin>>n;
	cout<<"Looking for clients"<<endl;
	listen(client,MAX);
	pthread_t read_t[MAX],write_t[MAX];
	
	while(1)
	{
		server[c]= accept(client, (struct sockaddr *)&server_addr, &size);
		if(server[c]<0)
		{
			cout<<"Error on accepting"<<endl;
			c++;
			return 0;
		}
		if(server[c]>0)
		{
			int p= recv(server[c],buffer,buffersize,0);
			if(p>=0)
			{
				printf("%s\n",buffer);
				send_to_all(buffer,buffersize,server[c]);
				pthread_create(&read_t[c],NULL,read_from_client,&server[c]);
			}
			else
			{
				cout<<"Client Disconnected";
			}
			//pthread_join(read_t[c], NULL);
			//pthread_create(&write_t[c],NULL,write_from_client,&server);
			c++;
		}
		if(c==n-1)
			break;
	}
		/*pthread_t read_t;
		pthread_create( &read_t, NULL, read_from_client, &server);
		cout<<"Read Thread Created \n";
		pthread_create( &write_t, NULL, write_from_client, (void *)server);
		cout<<"Write Thread Created \n";*/
	/*while(1)
	{
		gets(buffer);
		char s[buffersize];
		strcpy(s,"Server: ")
		strcat(s,buffer);
		strcpy(buffer,s);
		//printf("%s\n",buffer);
		for(int i=0; i<c; i++)
			send(server,buffer,buffersize,0);
		if(strcmp(buffer,"quit")==0)
			break;
	}*/
	for(int i=0; i<c; i++)
		pthread_join(read_t[i], NULL);
    	//pthread_cancel(write_t);
	cout<<"Connection terminated ";
	//close(client);
}
