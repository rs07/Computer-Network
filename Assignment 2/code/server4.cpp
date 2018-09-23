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
map<string,int> m;
map<string,int>::iterator it;
map<int,int> ser_id_one_one;
map<int,int>::iterator it1;

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

/*void show_online(int ser)
{
	for(it=m.begin(); it!=m.end(); it++)
	{
		char *buff;
		string name; 
		name= it->first;
		for(int i=0; i<name.length(); i++)
		{
			buff[i]=name[i];
		}
		it1= ser_id_one_one.find(it->second);
		if(it1!=ser_id_one_one.end())
		{
			strcat(buff," Free");
			send(ser,buff,20,0);
		}
		else
		{
			strcat(buff," Busy");
			send(ser,buff,20,0);
		}
	}
}*/

void show_id()
{
	cout<<"Showing id Details: \n";
	for(it= m.begin(); it!=m.end(); it++)
	{
		cout<<it->first<<" "<<it->second<<"\n";
	}
}

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
		//memset(server,-1,sizeof(server));
		m.clear();
		ser_id_one_one.clear();
		c--;
		close(ser);
		return;
	}
	for(int i=0; i<c; i++)
	{
		if(server[i]==ser)
		{
			close(ser);
			for(it=m.begin(); it!=m.end(); it++)
			{
				if(it->second==ser)
					break;
			}
			m.erase(it);
			int id2;
			id2= ser_id_one_one[ser];
			ser_id_one_one.erase(ser);
			ser_id_one_one.erase(id2);
			fflush(stdin);
			char *buffer="Disconnected";
			send(id2,buffer,12,0);
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
	string msg="";
	string na="";
	char *conn="Connect";
	char *qui="#quit#";
	char *onl="#Online#";
	int id1,id2;
	int flag=0,l=0,flag2=0;
	int z=0;
	while(1)
	{
		flag=0;
		flag2=0;
		recv(ser,buffer,buffersize,0);
		if(ser_id_one_one.find(ser)!=ser_id_one_one.end())
		{
			z=1;
		}
		msg.clear();
		
		for(it=m.begin(); it!=m.end(); it++)
		{
			if(it->second==ser)
			{
				na=it->first;
				break;
			}
		}
		l=0;
		for(int i=na.length()+2; i<na.length()+2+8; i++)
		{
			if(buffer[i]!=onl[l])
				break;
			l++;
		}
		if(l==8)
		{
			//show_online(ser);
			continue;
		}
		l=0;
		for(int i=na.length()+2; i<na.length()+2+6; i++)
		{
			if(buffer[i]!=qui[l])
			{
				flag2=1;
				break;
			}
			l++;
		}
		if(flag2==0)
		{
			//cout<<"Breaking \n";
			char *buff="quit";
			send(ser,buff,1024,0);
			break;
		}
		if(z==0)
		{
			
			cout<<na<<"\n";
			l=0;
			for(int i=na.length()+2; i<na.length()+7+2; i++)
			{
				if(buffer[i]!=conn[l])
				{
					cout<<buffer[i]<<" ";
					flag=1;
					break;
				}
				l++;
			}
			cout<<"\nFlag: "<<flag<<"\n";
		}
		if(flag==0||z==1)
		{
			if(z==0)
			{
				for(int i=na.length()+10; i<buffersize; i++)
				{
					if(buffer[i]==' ')
						break;
					msg = msg + buffer[i];
				}
				cout<<msg<<"\n";
				it=m.find(msg);
				if(it==m.end())
				{
					strcpy(buffer,"User Not Found");
					send(ser,buffer,buffersize,0);
					continue;
				}
				id2 = m[msg];
				cout<<"\nid2: "<<id2<<"\n";
				id1 = ser;
				ser_id_one_one[id1]=id2;
				ser_id_one_one[id2]=id1;
			}
			send(ser_id_one_one[ser],buffer,buffersize,0);
		}
	}
	quiting_server(ser);
}

int main()
{
	
	int buffersize=1024;
	char buffer[buffersize];
	int portNum=80,n;
	string name="";
	
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
		name.clear();
		server[c]= accept(client, (struct sockaddr *)&server_addr, &size);
		if(server[c]<0)
		{
			cout<<"Error on accepting"<<endl;
			c++;
			return 0;
		}
		if(server[c]>0)
		{
			int p = recv(server[c],buffer,buffersize,0);
			if(p>=0)
			{
				printf("%s\n",buffer);
				for(int i=0; i<buffersize; i++)
				{
					if(buffer[i]==' ')
					{
						break;
					}
					name=name + buffer[i];
				}
				m[name]=server[c];
				show_id();
				//cout<<"Name: "<<name<<"\n";
				send_to_all(buffer,buffersize,server[c]);
				pthread_create(&read_t[c],NULL,read_from_client,&server[c]);
			}
			else
			{
				cout<<"Client Disconnected";
			}
			//show_online(c);
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
