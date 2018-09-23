#include<bits/stdc++.h>
using namespace std;
#define MICRO 1000000

int main()
{
	unsigned short t= 56;
	unsigned char data[100];
	memcpy(&data[0], (unsigned char *)&t, sizeof(unsigned short));
	printf("%02X %02X\n",data[0],data[1]);
	data[2]=(unsigned char)t;
	printf("%02X\n",data[2]);
}





/*
sno = j;
        htons(sno);
        memcpy(&data_pac[offset], (unsigned char*)&sno, sizeof(unsigned short));
        offset = offset+2;        


        Storing the TIMESTAMP                 
        gettimeofday(&t1, NULL);                      //get current time
        t = (t1.tv_sec * 1000000 + t1.tv_usec);       //convert it into microseconds
        
        tsp = t - t_ref;                               //timestamp relative to the reference time
        htonl(tsp);
        memcpy(&data_pac[offset], (unsigned char*)tsp, sizeof(unsigned int));
        offset = offset+4;
*/