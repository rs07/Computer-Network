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
#include<fstream>
using namespace std;
#define MICRO 1000000
typedef long long ll;
//#define IP "127.0.0.1"


sockaddr_in server_addr;
socklen_t size;

int main(int argc, char *argv[])
{

	/*
	Assigning everything from Terminal to the variable
	*/
	int client,server;
	int port=atoi(argv[2]);
	int buffersize= atoi(argv[3]);
	unsigned char buffer[buffersize];
	int p= buffersize;
	int ttl= atoi(argv[4]);
	int num_packet= atoi(argv[5]);
	clock_t start,endt1,endt2;
	unsigned short c=1;
	int num_packet_send=0, num_packet_rec=0, total_rtt=0;
	ofstream myfile;
	myfile.open ("try.csv", ios::out | ios::app);
	client=socket(AF_INET,SOCK_DGRAM,0);
	if(client<0)
	{
		cout<<"Error creating socket "<<endl;
		exit(1);
	}
	cout<<"Client Socket Created ";
	size= sizeof(server_addr);
	start=clock();
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr= inet_addr(argv[1]);
	server_addr.sin_port=htons(port);
	{
		cout<<"=> Connection to the server "<<inet_ntoa(server_addr.sin_addr)<<" with port number : "<<port<<endl;
		//strcpy(buffer,"Client Connected");
		//printf("%s\n",buffer);
		char ttl_ch= (unsigned char)ttl;
		//cout<<"Enter the message: ";
		//scanf("%s",&buffer);
		//int len = strlen(buffer);
		//cout<<"Length: "<<len<<"\n";
		//sendto(client,buffer,buffersize,0,(struct sockaddr *)&server_addr, size);
		unsigned char th;
		th= (unsigned char)0;
		while(c<=num_packet)
		{
			/*   Declaration of packet  of size =============>  p + 7*/
			unsigned char data_packet[1400];


			/*   Getting the message into buffer  */


			/*for(int i=0; i<p; i++)
			{
				data_packet[7+i]= buffer[i];
			}*/


			/*	 Sequence of packet copying into data packet   */
			c= htons(c);
			memcpy(&data_packet[0],(unsigned char *)&c,sizeof(unsigned short));
			//printf("%02X %02X\n",data_packet[0],data_packet[1]);

			

			/*   For Calculating the time stamp */
			endt1= clock();
			unsigned int time_stamp= (double)(endt1-start)/CLOCKS_PER_SEC*MICRO;
			//printf("Send: %d\n",time_stamp);
			time_stamp= htonl(time_stamp);
			memcpy(&data_packet[2],(unsigned char *)&time_stamp,sizeof(unsigned int));
			

			/*	 For storing the ttl packet info in stream  */
			data_packet[6]= ttl_ch;
			//printf("%02X\n",data_packet[6]);
			//data_packet[7]='\0';

			
			/*	 For storing the message stored in the buffer  */
			//memcpy(&data_packet[7],(unsigned char)&buffer, sizeof(buffer));
			

			//cout<<"Client: ";
			//printf("%s\n",data_packet);
			char sq[2];
			while(1)
			{

				//cout<<"Hi\n";
				sendto(client,data_packet,p+7,0,(struct sockaddr *)&server_addr, size);
				++num_packet_send;
			
				int rec= recvfrom(client,data_packet,p+7,0,(struct sockaddr *)&server_addr, &size);
				endt2= clock();
				if(rec>=0)
				{
					++num_packet_rec;
					//cout<<"Server: ";
					//printf("%s\n",data_packet);
					//printf("%02X %02X %02X %02X %02X %02X\n",data_packet[0],data_packet[1],data_packet[2],data_packet[3],data_packet[4],data_packet[5]);
					//printf("%02X\n",data_packet[6]);
					//ttl_ch= data_packet[6];
					data_packet[6]--;
					if(data_packet[6]==th)
					{
						data_packet[6]= ttl_ch;
						break;
						/*endt2= clock();
						unsigned int time_stamp= (double)(endt2 - endt1)/CLOCKS_PER_SEC*MICRO;
						myfile<<time_stamp<<"\n";
						data_packet[6]=ttl;*/
					}
				}
				else
				{
					cout<<"Packet Drop\n";
				}
			}
			//printf("%REc: .10f\n",(double)(endt2-start)/CLOCKS_PER_SEC*MICRO);
			total_rtt= total_rtt + (double)(endt2- endt1)/CLOCKS_PER_SEC*MICRO;
			c++;
		}
		unsigned short b= total_rtt/num_packet_rec;
		//cout<<"Total Packet Sent: "<<num_packet_send<<"\n";
		//cout<<"Total Packet Recieved: "<<num_packet_rec<<"\n";
		cout<<"Average rtt : "<<b<<" "<<total_rtt/num_packet_rec<<"ms\n";
		myfile<<(total_rtt/num_packet_rec)<<"\n";
	}
	cout<<"Connection terminated \n";
	myfile.close();
	//close(client);
}
