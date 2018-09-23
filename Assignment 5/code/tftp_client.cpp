#include<iostream>
#include<bits/stdc++.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<netdb.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fstream>

using namespace std;

typedef long long ll;

#define MAX_INPUT_LEN 100
#define PACKET_SIZE 516

#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERR 5

#define MAXRETRY 5

char const *MODE="octet";

sockaddr_in server_addr;
int sockfd;
socklen_t server_len;


int get_socket()
{
	int sockid;
	sockid= socket(AF_INET,SOCK_DGRAM,0);
	if(sockid<0)
	{
		cout<<"Error creating socket "<<endl;
		exit(1);
	}
	return sockid;
}


void fill_struct(char *server_name, int port, sockaddr_in *server_adr)
{
	server_adr->sin_family=AF_INET;
	server_adr->sin_addr.s_addr= inet_addr(server_name);
	server_adr->sin_port=htons(port);
}

void start_end()
{
	//clrscr();
	cout<<"Processing ";
	for(int i=0; i<=100; i++)
	{
		cout<<"#";
		sleep(0.25);
	}
	//clrscr();
}

void display_help()
{
	//clrscr();
	cout<<"help- For going to help menu\n";
	cout<<"put- To send the file to the TFTP server\n";
	cout<<"get- To receive file from the TFTP server\n";
	cout<<"exit- To exiting from terminal\n";
	//cout<<"PRESS ANY KEY ...";
	sleep(1);
	//clrscr();
}

void send_file(char *server_name, char *filename)
{
	FILE *fp;
	fp= fopen(filename,"r");
	int len= strlen(filename);
	if(fp==NULL)
	{
		fprintf(stderr, "Error: Unable to find file\n");
		return;
	}

	char message[PACKET_SIZE], buffer[PACKET_SIZE];
	bzero(message,PACKET_SIZE);
	message[0]=0x0;
	message[1]=WRQ; //the WRQ opcode
	strcpy(message+2, filename);
	strcpy(message+2+len+1 , MODE);

	if (sendto(sockfd, message, PACKET_SIZE , 0 , (struct sockaddr *) &server_addr, server_len)==-1)
    {
            fprintf(stderr, "Error: Unable to send WRQ\n Timeout may be the reason\n");
    }
    printf("Sent WRQ.\n");

    int block_no=0;
    sockaddr_in upld_addr;
    socklen_t u_len= sizeof(upld_addr);

    while(1)
    {
    	int i;
    	int ackblock;
    	for(i=1;i<=MAXRETRY;i++)
    	{
    		bzero(buffer,PACKET_SIZE);
		    if (recvfrom(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &upld_addr, &u_len) == -1)
		    {
		        fprintf(stderr, "Error: Unable to recv ACK for block_no: %d\n Timeout may be the reason\n",block_no);
		    }
		    ackblock = (buffer[2]<<8) + buffer[3];

		    if((buffer[1]==ERR) || (ackblock==(block_no-1)))
		    {
		    	printf("Error sending blocknum, trying  again.%d\n", block_no);
		    	if (sendto(sockfd, message, PACKET_SIZE , 0 , (struct sockaddr *) &server_addr, server_len)==-1)
			    {
			    	fprintf(stderr, "Error: Unable to send file... Timeout may be the reason\n");
			    }

		    }
		    else
		    	break;
    	}
    	if(i>MAXRETRY)
    	{
    		printf("Giving up on sending file. :( \n" );
    		return ;
    	}

    	printf("ACK received for block number: %d\n",block_no);
    	fflush(stdout);

	    block_no++;
    	bzero(message, PACKET_SIZE);
    	message[1]=DATA;
    	message[2]=block_no>>8;
		message[3]=block_no%(0xff+1);

    	int p = fread(message+4 , 1 , 512 , fp);
    	//printf("%s\n",message+4 );
    	printf("Sending block %d of %d bytes.\n", block_no,p);
    	if (sendto(sockfd, message, p+4 , 0 , (struct sockaddr *) &upld_addr, u_len)==-1)
	    {
	            fprintf(stderr, "Error: Unable to send block : %d\n Timeout may be the reason\n",block_no);
	    }
    	
    	if(p<512)
    		break;
    }
    fclose(fp);
    printf("Upload complete.\n");
}



void recv_file(char *server_name, char *filename)
{
	FILE *fp;
	fp= fopen(filename,"w");
	char message[PACKET_SIZE], buffer[PACKET_SIZE];
	bzero(message,PACKET_SIZE);
	int len= strlen(filename);
	message[0]=0x0;
	message[1]=RRQ; //the RRQ opcode
	strcpy(message+2, filename);
	strcpy(message+2+len+1 , MODE);
	if (sendto(sockfd, message, PACKET_SIZE , 0 , (struct sockaddr *) &server_addr, server_len)==-1)
    {
            fprintf(stderr, "Error: Unable to send\n");
    }
    printf("Sent RRQ.\n");


    int block_no=1;
    sockaddr_in dwnld_addr;
    socklen_t d_addr_len= sizeof(dwnld_addr);

    while(1)
    {
    	bzero(buffer,PACKET_SIZE);
    	int p= recvfrom(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &dwnld_addr, &d_addr_len);
    	if (p == -1)
	    {
	        fprintf(stderr, "Error: Unable to Receive\n");
	    }

	    if(buffer[1]==ERR)
	    	fprintf(stderr,"Transferring Failed");

	    fwrite(&buffer[4],1,p-4,fp);
	    printf("Received number of block: %d\n",p);
	    bzero(message,PACKET_SIZE);
	    message[0]= 0x00;
	    message[1]= ACK;
	    message[2]= block_no>>8;
	    message[3]= block_no%(0xff+1);

		if (sendto(sockfd, message, 4 , 0 , (struct sockaddr *) &dwnld_addr, d_addr_len)==-1)
	    {
	            fprintf(stderr, "Error: Unable to send ACK\n");
	    }    	
    	printf("Sent ACK for block %d.\n", block_no);
    	block_no++;
    	if(p<512)
	    {
	    	break;
	    }
    }
    fclose(fp);
    printf("Download complete.\n");
}


int main(int argc, char *argv[])
{
	if(argc<2)
	{
		fprintf(stderr,"Usage: a.out <server_hostname>\n");
	}

	char *server_hostname= argv[1];
	server_len= sizeof(server_addr);
	sockfd= get_socket();
	fill_struct(server_hostname,69,&server_addr);
	struct timeval t_out;      
    t_out.tv_sec = 15;
    t_out.tv_usec = 0;

    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t_out,sizeof(t_out)) < 0)
    {
    	fprintf(stderr, "Error: Unable to set timer for receiving\n");
    }

    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&t_out,sizeof(t_out)) < 0)
    {
    	fprintf(stderr, "Error: Unable to set timer for sending\n");
    }

	start_end();
	display_help();

	char operation[MAX_INPUT_LEN], filename[MAX_INPUT_LEN];
	while(1)
	{
		cout<<"hercules@tftp>>";
		scanf("%s",&operation);
		printf("operation: %s\n",operation);
		if(strcmp(operation,"exit")==0)
		{
			start_end();
			cout<<"THANK YOU FOR VISITING... GOOD BYE";
			sleep(2);
			//clrscr();
			exit(0);
		}
		else if(strcmp(operation,"help")==0)
		{
			display_help();
		}
		else if(strcmp(operation,"put")==0)
		{
			scanf("%s",&filename);
			printf("Filename: %s\n",filename);
			send_file(server_hostname,filename);
		}
		else if(strcmp(operation,"get")==0)
		{
			scanf("%s",&filename);
			printf("Filename: %s\n",filename);
			recv_file(server_hostname,filename);
		}
		else
		{
			fprintf(stderr,"WRONG INPUT... PRESS HELP\n");
		}
	}
}