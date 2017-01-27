
/*========================================================
	Utility to create Modbus TCP protocol.
==========================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>

#define MODBUS_TCP_SRANDARD_PORT    502

int main(int argc, char **argv)
{
	int s;
	int i;
	struct sockaddr_in server;
	fd_set fds;
	struct timeval tv;
	unsigned char obuf[100];
	unsigned char ibuf[1024];

	unsigned char data[1024];
	char *temp = NULL;
	unsigned short slaveid, functionid, startreg, numofreg, LengthField, RegisterAddress, delay, queryint;
	unsigned char IPAddress[20];

	const char *filename = "Config.txt";

	FILE *fp = NULL;

	memset( data,'\0',1024);
	fp = fopen( filename, "r");
	if ( fp == NULL)
	{
		printf("\nKINDLY PUT THE Config.txt FILE IN THE CURRENT DIRECTORY AND RESTART THE APPLICATION\n\n");
		return 0;
	}	
	else
	{
		printf ("------File %s Opened Succesfully------\n", filename);
		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL )
		{
			temp++ ;
			slaveid = atoi(temp);
		}

		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL )
		{
			temp++ ;
			functionid  = atoi(temp);
		}

		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL )
		{
			temp++ ;
			startreg  = atoi(temp);
		}

		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL )
		{
			temp++ ;
			numofreg  = atoi(temp);
		}

		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL )
		{
			temp++ ;
			strcpy(IPAddress, temp);
		}

		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL )
		{
			temp++ ;
			LengthField  = atoi(temp);
		}

		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL )
		{
			temp++ ;
			RegisterAddress  = atoi(temp);
		}

		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL )
		{
			temp++ ;
			delay  = atoi(temp);
		}

		memset(data,'\0',sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL )
		{
			temp++ ;
			queryint  = atoi(temp);
		}

		fclose (fp);
	}

	printf(" slaveid=%d\n functionid=%d\n startreg=%d\n numofreg=%d\n LengthField=%d\n RegisterAddress=%d\n delay=%d\n queryint=%d\n IPAddress = %s\n", slaveid, functionid, startreg, numofreg, LengthField, RegisterAddress, delay, queryint, IPAddress);

	while(1)
	{
		/* establish connection to gateway on ASA standard port 502 */
		s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		server.sin_family = AF_INET;
		server.sin_port = htons(MODBUS_TCP_SRANDARD_PORT); /* ASA standard port */
		server.sin_addr.s_addr = inet_addr(IPAddress);

		i = connect(s, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
		if (i < 0)
		{
			printf("connect - error %d\n",i);
			close(s);
			return 1;
		}

		FD_ZERO(&fds);
		tv.tv_sec = delay;
		tv.tv_usec = 0;

		/* check ready to send */
		FD_SET(s, &fds);
		i = select(s+1, NULL, &fds, NULL, &tv);
		if (i<=0)
		{
			printf("select - error %d\n",i);
			close(s);
			return 1;
		}

		/* build MODBUS request */
		for (i=0;i<5;i++) obuf[i] = 0;

		obuf[5] = LengthField;
		obuf[6] = slaveid;
		obuf[7] = functionid;
		obuf[8] = (RegisterAddress >> 8) & 0xff;
		obuf[9] = RegisterAddress & 0xff;
		obuf[10] = (numofreg >> 8) & 0xff;
		obuf[11] = numofreg & 0xff;
		
		int count = 0;
		printf("command : ");
		for (count = 0; count < 12; count++)
			printf("0x%x ", obuf[count]);
		printf("\n");
		
		/* send request */
		i = send(s, obuf, 12, 0);
		if (i<12)
		{
			printf("failed to send all 12 chars\n");
		}

		/* wait for response */
		FD_SET(s, &fds);
		i = select(s+1, &fds, NULL, NULL, &tv);
		if (i<=0)
		{
			printf("no TCP response received\n");
			close(s);
			return 1;
		}

		/* receive and analyze response */
		i = recv(s, ibuf, 1024, 0);
		
		printf("response : ");
		for (count = 0; count < i; count++)
			printf("0x%x ", ibuf[count]);
		printf("\n");
			
		if (i < 9)
		{
			if (i==0)
			{
				printf("unexpected close of connection at remote end\n");
			}
			else
			{
				printf("response was too short - %d chars\n", i);
			}
		}
		else if (ibuf[7] & 0x80)
		{
			printf("MODBUS exception response - type %d\n", ibuf[8]);
		}
		else if (i != (9+2*numofreg))
		{
			printf("incorrect response size is %d expected %d\n",i,(9+2*numofreg));
		}
		else
		{
			for (i=0;i<numofreg;i++)
			{
				unsigned short w = (ibuf[9+i+i]<<8) + ibuf[10+i+i];
				printf("word %d = %d\n", i, w);
			}
		}
		/* close connection */
		close(s);
	}
}
