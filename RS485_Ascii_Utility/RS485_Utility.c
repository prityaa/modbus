
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <linux/rtc.h>
#include <errno.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <termios.h>
#include <time.h>
#include <dirent.h>

#include "rs485.h"


typedef unsigned short int UINT16;
typedef unsigned long int UINT32;
typedef signed short int INT16;
typedef signed long int INT32;
typedef unsigned char BYTE;

#define COMMAND_MID_THREE_BYTES 							"RP0"
#define COMMAND_FIRST_BYTE 								"$"


/****************************************************************************
Function:   Main Function
Description:
I/P Params:
Return:
*****************************************************************************/
void os_sleep_in_us(unsigned int usec)
{
	struct timespec sl_time = { 0, 0 };

#ifdef TESTING_WITH_PRINTF
	dbg_printf("sleeping for %u usec\n", usec);
#endif

	sl_time.tv_sec = (usec)/1000000;
	sl_time.tv_nsec = ((usec) % 1000000) * 1000;
	nanosleep(&sl_time, NULL);
}
#define os_sleep_in_ms(x)	os_sleep_in_us(x*1000)

void PrintHex(unsigned char *buf, int size)
{
	int i;
	for (i=0; i<size; i++)
	{
		printf("%02X ", buf[i]);
	}
	printf("\n");
}

void FilePrintHex(unsigned char *buf, int size, FILE *fp)
{
	//WriteFunctionLog("Entering");
	int i;
	for (i=0; i<size; i++)
	{
		fprintf(fp, "%02X,", buf[i]);
	}
	//fprintf(fp, "\r\n");
	//WriteFunctionLog("Exiting");
}

int my_atox ( char *s )
{

     int i=0;
     while ( *s )
     {
        if (*s >= 0x30 && *s <= 0x39)
             i = i*0x10 + (*s -0x30 );
        else if (*s >= 0x41 && *s <= 0x46)
             i = i*0x10 + (*s - 0x37 );
		else if(*s >= 0x61 && *s <= 0x66)
             i = i*0x10 + (*s - 0x57 );
		else
			return 0;//Failure
        s++;
     }

     return i;

 }

 /******************************** Constant ******************************************/

 const  BYTE auchCRCHi_exp[] = {
   0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
   0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
   0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01,
   0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41,
   0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81,
   0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0,
   0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01,
   0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,
   0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
   0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
   0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01,
   0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,
   0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
   0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
   0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01,
   0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,
   0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
   0x40
} ;

 const  BYTE auchCRCLo_exp[] = {
   0x00, 0xc0, 0xc1, 0x01, 0xc3, 0x03, 0x02, 0xc2, 0xc6, 0x06, 0x07, 0xc7, 0x05, 0xc5, 0xc4,
   0x04, 0xcc, 0x0c, 0x0d, 0xcd, 0x0f, 0xcf, 0xce, 0x0e, 0x0a, 0xca, 0xcb, 0x0b, 0xc9, 0x09,
   0x08, 0xc8, 0xd8, 0x18, 0x19, 0xd9, 0x1b, 0xdb, 0xda, 0x1a, 0x1e, 0xde, 0xdf, 0x1f, 0xdd,
   0x1d, 0x1c, 0xdc, 0x14, 0xd4, 0xd5, 0x15, 0xd7, 0x17, 0x16, 0xd6, 0xd2, 0x12, 0x13, 0xd3,
   0x11, 0xd1, 0xd0, 0x10, 0xf0, 0x30, 0x31, 0xf1, 0x33, 0xf3, 0xf2, 0x32, 0x36, 0xf6, 0xf7,
   0x37, 0xf5, 0x35, 0x34, 0xf4, 0x3c, 0xfc, 0xfd, 0x3d, 0xff, 0x3f, 0x3e, 0xfe, 0xfa, 0x3a,
   0x3b, 0xfb, 0x39, 0xf9, 0xf8, 0x38, 0x28, 0xe8, 0xe9, 0x29, 0xeb, 0x2b, 0x2a, 0xea, 0xee,
   0x2e, 0x2f, 0xef, 0x2d, 0xed, 0xec, 0x2c, 0xe4, 0x24, 0x25, 0xe5, 0x27, 0xe7, 0xe6, 0x26,
   0x22, 0xe2, 0xe3, 0x23, 0xe1, 0x21, 0x20, 0xe0, 0xa0, 0x60, 0x61, 0xa1, 0x63, 0xa3, 0xa2,
   0x62, 0x66, 0xa6, 0xa7, 0x67, 0xa5, 0x65, 0x64, 0xa4, 0x6c, 0xac, 0xad, 0x6d, 0xaf, 0x6f,
   0x6e, 0xae, 0xaa, 0x6a, 0x6b, 0xab, 0x69, 0xa9, 0xa8, 0x68, 0x78, 0xb8, 0xb9, 0x79, 0xbb,
   0x7b, 0x7a, 0xba, 0xbe, 0x7e, 0x7f, 0xbf, 0x7d, 0xbd, 0xbc, 0x7c, 0xb4, 0x74, 0x75, 0xb5,
   0x77, 0xb7, 0xb6, 0x76, 0x72, 0xb2, 0xb3, 0x73, 0xb1, 0x71, 0x70, 0xb0, 0x50, 0x90, 0x91,
   0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9c, 0x5c,
   0x5d, 0x9d, 0x5f, 0x9f, 0x9e, 0x5e, 0x5a, 0x9a, 0x9b, 0x5b, 0x99, 0x59, 0x58, 0x98, 0x88,
   0x48, 0x49, 0x89, 0x4b, 0x8b, 0x8a, 0x4a, 0x4e, 0x8e, 0x8f, 0x4f, 0x8d, 0x4d, 0x4c, 0x8c,
   0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
   0x40
} ;

// Procedure to check the checksum of the message
//Function Name - MH10
UINT16 modrtuChecksum ( BYTE *chkbuf, BYTE len )
{
   BYTE uchCRCHi = 0xff ;
   BYTE uchCRCLo = 0xff ;
   UINT16 uIndex ;
   UINT16 temp_code;

   while ( len )
      {
      uIndex = (UINT16) ( uchCRCHi ^ *chkbuf++ ) ;

      uchCRCHi = (BYTE) ( uchCRCLo ^ auchCRCHi_exp[uIndex] ) ;
      uchCRCLo = auchCRCLo_exp[uIndex] ;
      len-- ;
      }
   temp_code = (UINT16) uchCRCHi;
   temp_code = (UINT16) (temp_code << 8);

   return(UINT16) (  temp_code | uchCRCLo ) ;
}

// Builds the modbus message
//Function Name - MH9.
void BuildModbusCmd( BYTE node_id, BYTE function, UINT16 address, INT16 amount,BYTE UART_TX_Buffer[])
{
   UINT16 crc ;
   //  NODE ID TO MESSAGE
   UART_TX_Buffer[0]=node_id;
   //  FUNCTION TO MESSAGE
   UART_TX_Buffer[1]=function;
   //ADDRESS TO MESSAGE HIGH THEN LOW
   UART_TX_Buffer[2]=(BYTE)((address>>8)&0xff) ;
   UART_TX_Buffer[3]=(BYTE)((address)&0xff) ;
   //LENGTH/DATA TO MESSAGE HIGH THEN LOW
   UART_TX_Buffer[4]=(BYTE)((amount>>8)&0xff) ;
   UART_TX_Buffer[5]=(BYTE)((amount)&0xff) ;
   //CHECKSUM TO MESSAGE  HIGH THEN LOW
   crc = modrtuChecksum (UART_TX_Buffer, 6) ;
   UART_TX_Buffer[6]=(BYTE)((crc >> 8) & 0xff) ;
   UART_TX_Buffer[7]=(BYTE)((crc) & 0xff) ;
}

void GetTime(char *CurrTime,int formatted,int CurrTimeLen)
{	
	
	struct tm *gm_time;

	struct timeval currentTime;
	
	gettimeofday(&currentTime, NULL);
	
	gm_time = gmtime(&currentTime.tv_sec);
		
	switch (formatted) {
	case 1:
		//Date Time format for creating date time for Protocol.
		snprintf(CurrTime,CurrTimeLen,"%02d%02d%02d%02d%02d%02d", 
			gm_time->tm_mday, gm_time->tm_mon+1,(gm_time->tm_year < 100) ? gm_time->tm_year : gm_time->tm_year-100,  
			gm_time->tm_hour, gm_time->tm_min, gm_time->tm_sec);
		break;	
	case 2:	
		//Date Time format for putting the time in the log file.
		snprintf(CurrTime,CurrTimeLen,"%02d-%02d-%02d,%02d:%02d:%02d", 
			gm_time->tm_mday, gm_time->tm_mon+1,(gm_time->tm_year < 100) ? gm_time->tm_year : gm_time->tm_year-100,
			gm_time->tm_hour, gm_time->tm_min, gm_time->tm_sec);
		break;		
	}	
}

int CalculateDataChecksum(char *buffer,int buflen)
{
		
	int checksum = 0,i=0;	
	//Calculate check sum for SMS data
	for(i=0;i<buflen;i++)
	{
		checksum = checksum ^ buffer[i];
	}

	printf("Calculated Checksum = %d\n",checksum);
	return checksum;
}

int main(int argc, char *argv[])
{
    uint port = 3, delay = 500, parity = 0, baudrate = 9600, databits = 8, stopbits = 1, flowcontrol = 0, queryint = 500;
	uint slaveid = 50, functionid = 3, startreg = 30000, numofreg = 26;
    char CurrTime[20];
    unsigned char data[1024];
	unsigned char command[100];
	unsigned char dataread[1024];
	unsigned char Tmpslaveid[50];
	unsigned char command_crc[100];
	unsigned char crc[100];
	unsigned int tx = 0, rx = 0;
	const char *filename = "Config.txt";
    const char *filewrite = "Log.txt";
    int ret = 0, retrycnt = 0;
    int check=0, len = 0;
    FILE *fp = NULL;
    FILE *fs = NULL;
	char *temp = NULL;
    
	memset( data,'\0',1024);
	
	printf(" SlaveID=%d\n FunctionID=%d\n StartRegister=%d\n No.ofRegisters=%d\n Port=%d\n Baudrate=%d\n Parity=%d\n Databits=%d\n Stopbits=%d\n Flowcontrol=%d\n Delay=%d\n Query Interval=%d\n", slaveid, functionid, startreg, numofreg, port, baudrate, parity, databits, stopbits, flowcontrol, delay, queryint);
	
	// Command building
	memset( command,'\0',100);
	snprintf(Tmpslaveid,5,"%02d",slaveid);

	//Building for CRC sending
	memset(command_crc,0,sizeof(command_crc));
	strcpy(command_crc,Tmpslaveid);
	strcat(command_crc,COMMAND_MID_THREE_BYTES);

	//Sending to CRC
	len=strlen(command_crc);
	check = CalculateDataChecksum(command_crc, len);
	printf("%d\n", check);
	snprintf(crc,5,"%02x",check);
	len=strlen(crc);
	crc[len] = '\0';

	//Building Command
	memset(command,0,sizeof(command));
	strcpy(command,COMMAND_FIRST_BYTE);
	strcat(command,Tmpslaveid);
	strcat(command,COMMAND_MID_THREE_BYTES);
	strcat(command,crc);
	strcat(command,"\r");
	len=strlen(command);
	printf ( " The Built Command = %s \n ",command ) ;
	
	BuildModbusCmd(slaveid,functionid,startreg,numofreg,command);

	printf("mod bus command : %s\n", command);
	
	while(1)
    {	
		if( (RS485OpenPort(port) ) == FAILURE)
		{
			printf("\n Port Open Failed\n");
			return 0;
		}
		if((RS485Port_Initialize(port, baudrate, flowcontrol, parity, databits, stopbits,delay))==SUCCESS)
		{
		   printf("\n\n PORT %d INITIALIZATION SUCCESSFUL\n", port);
		}
		else
		{
			printf("\n\n PORT %d INITIALIZATION FAILED\n", port);
			RS485ClosePort(port);
			return 0;
		}			
	
		ret = RS485Port_Write_Data(port, command,8);
		if(ret <= SUCCESS)
		{
			printf(" WRITE FAILED\n");
		}
		else
		{
			printf(" Command sent = %s\n", command);
			PrintHex(command,8);
			printf(" Write bytes = %d\n",ret);
			
		}
		
		memset(dataread,0,sizeof(dataread));
		
		ret = RS485Port_Read_Data(port,dataread,1024);
		if(ret > 0)
		{
			
			printf(" Read bytes = %d\n",ret);
			printf("response from meter : ");
			PrintHex(dataread,ret);
			
		}
		else
		{
			printf(" Read Failed\n");
		}
		os_sleep_in_ms(queryint);
		RS485ClosePort(port);
	}
	
    return 0;
}

