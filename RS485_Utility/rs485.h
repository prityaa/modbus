#ifndef __RS485_H
#define __RS485_H

#define FAILURE -1
#define SUCCESS 0
#define TRUE 1
#define FALSE 0

extern int RS485Port_Initialize(uint port, ulong baud_rate, uint flow_control, uint parity, uint data_bits, uint stop_bits, uint delay);
extern int RS485Port_Write_Data(int port, unsigned char *Data, uint length_in);
extern int RS485Port_Read_Data(int port,unsigned char *Data, uint length_exp);
extern int RS485OpenPort(uint port);
extern int RS485ClosePort(uint port);


#endif
