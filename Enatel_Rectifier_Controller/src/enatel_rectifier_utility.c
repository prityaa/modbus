
/**========================================================
	*  INVENDIS TECHNOLOGY
	*  Utility to ENATEL mains monitor controller 
	*  based on Modbus TCP protocol.
	*  More info, Kindly read readme file.
==========================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include "../include/enatel_rectifier_utility.h"

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
	for (i=0; i < size; i++)
		pr_debug("0x%02X ", buf[i]);
		
	pr_debug("\n");
}

void build_tcp_ip_modbus_command(unsigned char *tcp_iP_modbus_command, 
							unsigned short slaveid, 
							unsigned short functionid, 
							unsigned short startreg, 
							unsigned short numofreg, 
							int command_num) 
{
	pr_debug("build_tcp_ip_modbus_command : command_num = %d, slaveid = %d,\
				functionid = %d, startreg = %d, numofreg = %d\n",
				command_num, slaveid, functionid, startreg, numofreg);	
				
	tcp_iP_modbus_command[5] = MODBUS_TCP_DATA_FIELD;
	tcp_iP_modbus_command[6] = slaveid;
	tcp_iP_modbus_command[7] = functionid;
	tcp_iP_modbus_command[8] = (startreg >> 8) & 0xff;
	tcp_iP_modbus_command[9] = startreg & 0xff;
	tcp_iP_modbus_command[10] = (numofreg >> 8) & 0xff;
	tcp_iP_modbus_command[11] = numofreg & 0xff;
	PrintHex(tcp_iP_modbus_command, 12);
}

void parse_enatel_mains_monitor_alarms(const char *ibuf, const int ret, char *alarms)
{
	int response_byte = 0, alarms_count = 0, i = 0;
	
	memset(alarms, '\0', sizeof(alarms));
	for (response_byte = 9; response_byte < ret; response_byte++)  {
		pr_debug("ibuf[%d]  = %x\n", response_byte, ibuf[response_byte] );
		for (i = 0; i < 8; i++, alarms_count++)  {
			if (ibuf[response_byte] & 1 << i) 
				alarms[alarms_count] = 1;
			pr_debug("alarms[%d]  = %x\n", alarms_count, alarms[alarms_count] );	
		}
	}
}

void display_enatel_mains_monitor_alarms(char *alarms)
{
	int usr_alm_cnt = 0;
	pr_debug("display_enatel_mains_monitor_alarms : printing all parameters\n");
	
	printf("Sr.No\tAlarms Name\t\t\t\tAlarms\n");
	for (usr_alm_cnt = 0; usr_alm_cnt < 32; usr_alm_cnt++)
		printf(" %02d . \tUser Alarm %d \t\t\t\t %s\n", 
				usr_alm_cnt+1, usr_alm_cnt+1, alarms[usr_alm_cnt] ? "HIGH" : "LOW ");
	
	printf(" 33 . \tRectifier Mains Fail \t\t\t %s\n", alarms[32] ? "HIGH" : "LOW ");
	printf(" 34 . \tRectifier Over Temperature  \t\t %s\n", alarms[34] ? "HIGH" : "LOW ");
	printf(" 35 . \tRectifier Fan Fail \t\t\t %s\n", alarms[35] ? "HIGH" : "LOW ");
	printf(" 36 . \tRectifier Current Limit\t\t\t %s\n", alarms[36] ? "HIGH" : "LOW ");
	printf(" 37 . \tRectifier Over Voltage\t\t\t %s\n", alarms[37] ? "HIGH" : "LOW ");
	printf(" 38 . \tRectifier Brownout\t\t\t %s\n", alarms[38] ? "HIGH" : "LOW ");
	printf(" 39 . \tRectifier Postmate Open\t\t\t %s\n", alarms[39] ? "HIGH" : "LOW ");
	printf(" 40 . \tRectifier Temperature Sense Fail\t %s\n", alarms[41] ? "HIGH" : "LOW ");
	printf(" 41 . \tRectifier Shutdown\t\t\t %s\n", alarms[45] ? "HIGH" : "LOW ");
	printf(" 42 . \tRectifier EEPROM Fail\t\t\t %s\n", alarms[46] ? "HIGH" : "LOW ");
	printf(" 43 . \tRectifier Soft Starting\t\t\t %s\n", alarms[47] ? "HIGH" : "LOW ");
	printf(" 44 . \tConverter Input Fail\t\t\t %s\n", alarms[48] ? "HIGH" : "LOW ");
	printf(" 45 . \tConverter Over Temperature\t\t %s\n", alarms[50] ? "HIGH" : "LOW ");
	printf(" 46 . \tConverter Fan Fail\t\t\t %s\n", alarms[51] ? "HIGH" : "LOW ");
	printf(" 47 . \tConverter Current Limit\t\t\t %s\n", alarms[52] ? "HIGH" : "LOW ");
	printf(" 48 . \tConverter Over Voltage\t\t\t %s\n", alarms[53] ? "HIGH" : "LOW ");
	printf(" 49 . \tConverter Brownout\t\t\t %s\n", alarms[54] ? "HIGH" : "LOW ");
	printf(" 50 . \tConverter Postmate Open\t\t\t %s\n", alarms[55] ? "HIGH" : "LOW ");
	printf(" 51 . \tConverter Temperature Sense Fail\t %s\n", alarms[57] ? "HIGH" : "LOW ");
	printf(" 52 . \tConverter Shutdown\t\t\t %s\n", alarms[61] ? "HIGH" : "LOW ");
	printf(" 53 . \tConverter EEPROM Fail\t\t\t %s\n", alarms[62] ? "HIGH" : "LOW ");
	printf(" 54 . \tConverter Soft Starting\t\t\t %s\n", alarms[63] ? "HIGH" : "LOW ");
	printf(" 55 . \tInverter Communication Fail\t\t %s\n", alarms[64] ? "HIGH" : "LOW ");
	printf(" 56 . \tInverter Over Temp\t\t\t %s\n", alarms[66] ? "HIGH" : "LOW ");
	printf(" 57 . \tInverter Fan Fail\t\t\t %s\n", alarms[67] ? "HIGH" : "LOW ");
	printf(" 58 . \tInverter Power Limit\t\t\t %s\n", alarms[68] ? "HIGH" : "LOW ");
	printf(" 59 . \tInverter Input Volts High\t\t %s\n", alarms[69] ? "HIGH" : "LOW ");
	printf(" 60 . \tInverter Input Volts Low\t\t %s\n", alarms[70] ? "HIGH" : "LOW ");
	printf(" 61 . \tInverter Emergency Power Off\t\t %s\n", alarms[71] ? "HIGH" : "LOW ");
	printf(" 62 . \tInverter Output Voltage High\t\t %s\n", alarms[72] ? "HIGH" : "LOW ");
	printf(" 63 . \tInverter Output Voltage Low\t\t %s\n", alarms[73] ? "HIGH" : "LOW ");
	printf(" 64 . \tInverter Negative Power Protection\t %s\n", alarms[74] ? "HIGH" : "LOW ");
	printf(" 65 . \tInverter Output Volts Abnormal\t\t %s\n", alarms[75] ? "HIGH" : "LOW ");
	printf(" 66 . \tInverter Sync Pulse Faul\t\t %s\n", alarms[76] ? "HIGH" : "LOW ");
	printf(" 67 . \tInverter Shutdown\t\t\t %s\n", alarms[77] ? "HIGH" : "LOW ");
	printf(" 68 . \tInverter EEPROM Fail\t\t\t %s\n", alarms[78] ? "HIGH" : "LOW ");
	printf(" 69 . \tInverter Soft Start Fail\t\t %s\n", alarms[79] ? "HIGH" : "LOW ");
	printf(" 70 . \tRectifier Non-Urgent Fail\t\t %s\n", alarms[80] ? "HIGH" : "LOW ");
	printf(" 71 . \tRectifier Urgent Fail\t\t\t %s\n", alarms[81] ? "HIGH" : "LOW ");
	printf(" 72 . \tAll Rectifiers Failed\t\t\t %s\n", alarms[82] ? "HIGH" : "LOW ");
	printf(" 73 . \tUrgent Rectifier Missing\t\t %s\n", alarms[83] ? "HIGH" : "LOW ");
	printf(" 74 . \tNon-Urgent Rectifier Missing\t\t %s\n", alarms[84] ? "HIGH" : "LOW ");
	printf(" 75 . \tConverter Non-Urgent Fail\t\t %s\n", alarms[85] ? "HIGH" : "LOW ");
	printf(" 76 . \tConverter Urgent Fail\t\t\t %s\n", alarms[86] ? "HIGH" : "LOW ");
	printf(" 77 . \tAll Converters Failed\t\t\t %s\n", alarms[87] ? "HIGH" : "LOW ");
	printf(" 78 . \tConverter Missing\t\t\t %s\n", alarms[88] ? "HIGH" : "LOW ");
	printf(" 79 . \tInverter Non-Urgent Fail\t\t %s\n", alarms[90] ? "HIGH" : "LOW ");
	printf(" 80 . \tInverter Urgent Fail\t\t\t %s\n", alarms[91] ? "HIGH" : "LOW ");
	printf(" 81 . \tAll Inverters Fail\t\t\t %s\n", alarms[92] ? "HIGH" : "LOW ");
	printf(" 82 . \tInverter Missing\t\t\t %s\n", alarms[93] ? "HIGH" : "LOW ");
	printf(" 83 . \tLow Voltage Bus 1\t\t\t %s\n", alarms[96] ? "HIGH" : "LOW ");
	printf(" 84 . \tLow Float Bus 1\t\t\t\t %s\n", alarms[97] ? "HIGH" : "LOW ");
	printf(" 85 . \tHigh Voltage Bus 1\t\t\t %s\n", alarms[98] ? "HIGH" : "LOW ");
	printf(" 86 . \tHigh Float Bus 1\t\t\t %s\n", alarms[99] ? "HIGH" : "LOW ");
	printf(" 87 . \tLow Voltage Bus 2\t\t\t %s\n", alarms[100] ? "HIGH" : "LOW ");
	printf(" 88 . \tLow Float Bus 2\t\t\t\t %s\n", alarms[101] ? "HIGH" : "LOW ");
	printf(" 89 . \tHigh Voltage Bus 2\t\t\t %s\n", alarms[102] ? "HIGH" : "LOW ");
	printf(" 90 . \tHigh Float Bus 2\t\t\t %s\n", alarms[103] ? "HIGH" : "LOW ");
	printf(" 91 . \tLVD 1 Operate\t\t\t\t %s\n", alarms[104] ? "HIGH" : "LOW ");
	printf(" 92 . \tLVD 2 Operate\t\t\t\t %s\n", alarms[105] ? "HIGH" : "LOW ");
	printf(" 93 . \tBattery Cell Voltage Symmetry\t\t %s\n", alarms[106] ? "HIGH" : "LOW ");
	printf(" 94 . \tLVD 3 Operate\t\t\t\t %s\n", alarms[107] ? "HIGH" : "LOW ");
	printf(" 95 . \tSystem Load Current High\t\t %s\n", alarms[112] ? "HIGH" : "LOW ");
	printf(" 96 . \tBattery Current High\t\t\t %s\n", alarms[114] ? "HIGH" : "LOW ");
	printf(" 97 . \tBattery String Current High\t\t %s\n", alarms[115] ? "HIGH" : "LOW ");
	printf(" 98 . \tBattery String Current Imbalance\t %s\n", alarms[116] ? "HIGH" : "LOW ");
	printf(" 99 . \tBattery String Open\t\t\t %s\n", alarms[117] ? "HIGH" : "LOW ");
	printf(" 100 . \tBattery Temperature Low\t\t\t %s\n", alarms[118] ? "HIGH" : "LOW ");
	printf(" 101 . \tBattery Temperature High\t\t %s\n", alarms[119] ? "HIGH" : "LOW ");
	printf(" 102 . \tBattery Temperature Sensor Faul\t\t %s\n", alarms[120] ? "HIGH" : "LOW ");
	printf(" 103 . \tAmbient Temperature Low\t\t\t %s\n", alarms[121] ? "HIGH" : "LOW ");
	printf(" 104 . \tAmbient Temperature High\t\t %s\n", alarms[122] ? "HIGH" : "LOW ");
	printf(" 105 . \tAmbient Temperature Sensor Faul\t\t %s\n", alarms[123] ? "HIGH" : "LOW ");
	printf(" 106 . \tBattery MCB 1 open\t\t\t %s\n", alarms[124] ? "HIGH" : "LOW ");
	printf(" 107 . \tBattery MCB 2 open\t\t\t %s\n", alarms[125] ? "HIGH" : "LOW ");
	printf(" 108 . \tBattery MCB 3 open\t\t\t %s\n", alarms[126] ? "HIGH" : "LOW ");
	printf(" 119 . \tBattery MCB 4 open\t\t\t %s\n", alarms[127] ? "HIGH" : "LOW ");
	printf(" 110 . \tPower Save Active\t\t\t %s\n", alarms[128] ? "HIGH" : "LOW ");
	printf(" 111 . \tPeriodic Equalise\t\t\t %s\n", alarms[129] ? "HIGH" : "LOW ");
	printf(" 112 . \tManual Equalise\t\t\t\t %s\n", alarms[130] ? "HIGH" : "LOW ");
	printf(" 113 . \tFast Charge\t\t\t\t %s\n", alarms[131] ? "HIGH" : "LOW ");
	printf(" 114 . \tBattery Test\t\t\t\t %s\n", alarms[132] ? "HIGH" : "LOW ");
	printf(" 115 . \tBattery Discharge\t\t\t %s\n", alarms[133] ? "HIGH" : "LOW ");
	printf(" 116 . \tBattery Test Fail\t\t\t %s\n", alarms[134] ? "HIGH" : "LOW ");
	printf(" 117 . \tBattery Current Limit Active\t\t %s\n", alarms[135] ? "HIGH" : "LOW ");
	printf(" 118 . \tSystem Current Limit Active\t\t %s\n", alarms[136] ? "HIGH" : "LOW ");
	printf(" 119 . \tPeripheral Missing\t\t\t %s\n", alarms[137] ? "HIGH" : "LOW ");
	printf(" 120 . \tLVD Fail\t\t\t\t %s\n", alarms[138] ? "HIGH" : "LOW ");
	printf(" 121 . \tMonitor ADC Fail\t\t\t %s\n", alarms[139] ? "HIGH" : "LOW ");
	printf(" 122 . \tLogic Error\t\t\t\t %s\n", alarms[140] ? "HIGH" : "LOW ");
	printf(" 123 . \tMonitor Fan Fail\t\t\t %s\n", alarms[141] ? "HIGH" : "LOW ");
	printf(" 124 . \tSystem Power Limit Active\t\t %s\n", alarms[142] ? "HIGH" : "LOW ");
	printf(" 125 . \tAC Voltage High\t\t\t\t %s\n", alarms[144] ? "HIGH" : "LOW ");
	printf(" 126 . \tAC Voltage Low\t\t\t\t %s\n", alarms[145] ? "HIGH" : "LOW ");
	printf(" 127 . \tAC Phase Lost\t\t\t\t %s\n", alarms[146] ? "HIGH" : "LOW ");
	printf(" 128 . \tAC Current High\t\t\t\t %s\n", alarms[147] ? "HIGH" : "LOW ");
	printf(" 129 . \tAC Frequency High\t\t\t %s\n", alarms[148] ? "HIGH" : "LOW ");
	printf(" 130 . \tStatic Bypass Control Power Fail\t %s\n", alarms[160] ? "HIGH" : "LOW ");
	printf(" 131 . \tStatic Bypass Unavailable\t\t %s\n", alarms[161] ? "HIGH" : "LOW ");
	printf(" 132 . \tStatic Bypass Over Temperature\t\t %s\n", alarms[162] ? "HIGH" : "LOW ");
	printf(" 133 . \tStatic Bypass Fan Fail\t\t\t %s\n", alarms[163] ? "HIGH" : "LOW ");
	printf(" 134 . \tStatic Bypass Power Limi\t\t %s\n", alarms[164] ? "HIGH" : "LOW ");
	printf(" 135 . \tStatic Bypass Switch Fail\t\t %s\n", alarms[165] ? "HIGH" : "LOW ");
	printf(" 136 . \tStatic Bypass Priority Alarm\t\t %s\n", alarms[166] ? "HIGH" : "LOW ");
	printf(" 137 . \tStatic Bypass Back-Feed Relay Open\t %s\n", alarms[167] ? "HIGH" : "LOW ");
	printf(" 138 . \tStatic Bypass SCR Short Circuit\t\t %s\n", alarms[168] ? "HIGH" : "LOW ");
	printf(" 139 . \tStatic Bypass Output Short Circuit\t %s\n", alarms[169] ? "HIGH" : "LOW ");
	printf(" 140 . \tStatic Bypass Running In Fault Mode\t %s\n", alarms[170] ? "HIGH" : "LOW ");
	printf(" 141 . \tStatic Bypass Mode Alarm\t\t %s\n", alarms[171] ? "HIGH" : "LOW ");
	printf(" 142 . \tStatic Bypass Inverter Unavailable\t %s\n", alarms[172] ? "HIGH" : "LOW ");
	printf(" 143 . \tStatic Bypass Mains Unavailable\t\t %s\n", alarms[173] ? "HIGH" : "LOW ");
	printf(" 144 . \tStatic Bypass EEPROM Fail\t\t %s\n", alarms[174] ? "HIGH" : "LOW ");
	printf(" 145 . \tStatic Bypass Manual Bypass \n\tSwitch Abnormal Position\t\t %s\n", alarms[175] ? "HIGH" : "LOW ");
	printf("\n");
}
void display_enatel_mains_monitor_data(
			const struct enatel_mains_rectifier_controller *s_rectifier_controller)
{
	char a;
	int i = 0;
	pr_debug("display_enatel_mains_monitor_data : printing all parameters\n");
	
	printf("Sr.No\tParameter\t\t\t\tValue\n");
	printf(" 1 . \tBus Voltage 1 \t\t\t\t%f\n", s_rectifier_controller->bus_voltage);
	printf(" 2 . \tNumber Of Rectifiers  \t\t\t%d\n", s_rectifier_controller->num_of_rectifiers);
	printf(" 3 . \tRectifier Output Current  \t\t%d\n", s_rectifier_controller->rect_output_curr);
	printf(" 4 . \tLoad Current  \t\t\t\t%d\n", s_rectifier_controller->load_curr);
	printf(" 5 . \tBattery Current  \t\t\t%d\n", s_rectifier_controller->batt_curr);
	printf(" 6 . \tBattery Capacity Remaining  \t\t%d\n", s_rectifier_controller->batt_capacity_remaining);
	printf(" 7 . \tBattery Discharge Time Remaining  \t%d\n", s_rectifier_controller->battery_discharget_time_remaining);
	printf(" 8 . \tBattery Temperature  \t\t\t%f\n", s_rectifier_controller->batt_temperature);
	printf(" 9 . \tAmbient Temperature  \t\t\t%f\n", s_rectifier_controller->ambient_temperature);
	printf("10 . \tNumber of 12V Converters  \t\t%d\n", s_rectifier_controller->num_of_12V_converters);
	printf("11 . \t12V Converter Output Voltage  \t\t%f\n", s_rectifier_controller->_12V_converter_output_volt);
	printf("12 . \t12V Converter Output Current  \t\t%d\n", s_rectifier_controller->_12V_converter_output_curr);
	printf("13 . \tNumber of 24V Converters  \t\t%d\n", s_rectifier_controller->num_of_24V_converters);
	printf("14 . \t24V Converter Output Voltage  \t\t%f\n", s_rectifier_controller->_24V_converter_output_volt);
	printf("15 . \t24V Converter Output Current  \t\t%d\n", s_rectifier_controller->_24V_converter_output_curr);
	printf("16 . \tNumber of 48V Converters  \t\t%d\n", s_rectifier_controller->num_of_48V_converters);
	printf("17 . \t48V Converter Output Voltage  \t\t%f\n", s_rectifier_controller->_48V_converter_output_volt);
	printf("18 . \t48V Converter Output Current  \t\t%d\n", s_rectifier_controller->_48V_converter_output_curr);
	printf("19 . \tNumber of 60V Converters  \t\t%d\n", s_rectifier_controller->num_of_60V_converters);
	printf("20 . \t60V Converter Output Voltage \t\t%f\n", s_rectifier_controller->_60V_converter_output_volt);
	printf("21 . \t60V Converter Output Current  \t\t%d\n", s_rectifier_controller->_60V_converter_output_curr);
	printf("22 . \tNumber of Inverters  \t\t\t%d\n", s_rectifier_controller->number_of_inverters);
	printf("23 . \tInverter Output AC Voltage  \t\t%f\n", s_rectifier_controller->inverter_output_ac_volt);
	printf("24 . \tInverter Output AC Current  \t\t%f\n", s_rectifier_controller->inverter_output_ac_curr);
	printf("25 . \tFan Speed 1  \t\t\t\t%f\n", s_rectifier_controller->fan_speed_1);
	printf("26 . \tFan Speed 2  \t\t\t\t%f\n", s_rectifier_controller->fan_speed_2);
	for (; i < 16; i++)
		printf("%d . \tCustom Variable %d  \t\t\t%d\n", i+27, i+1, s_rectifier_controller->custom_variable[i]);
	
	for (i = 0; i < 3; i++)
		printf("%d . \tMains Monitor 1 AC Current %d \t\t%f\n", 43+i, i+1, s_rectifier_controller->mains_monitor_1_ac_voltage[i]);
	for (i = 0; i < 3; i++)
		printf("%d . \tMains Monitor 1 AC Voltage %d \t\t%f\n", 46+i, i+1, s_rectifier_controller->mains_monitor_1_ac_curr[i]);
	printf("49 . \tMains Monitor 1 Frequency  \t\t%d\n",  s_rectifier_controller->mains_moninitor_1_frequency);
	
	for (i = 0; i < 3; i++)
		printf("%d . \tMains Monitor 2 AC Voltage %d \t\t%f\n", 50+i, i+1, s_rectifier_controller->mains_monitor_2_ac_voltage[i]);
	for (i = 0; i < 3; i++)
		printf("%d . \tMains Monitor 2 AC Current %d \t\t%f\n", 53+i, i+1, s_rectifier_controller->mains_monitor_2_ac_curr[i]);
	printf("54 . \tMains Monitor 2 Frequency  \t\t%d\n", s_rectifier_controller->mains_moninitor_2_frequency);
	
	printf("55 . \tFloat Voltage \t\t\t\t%f\n", s_rectifier_controller->float_voltage);
	printf("56 . \tFast Charge Voltage \t\t\t%f\n", s_rectifier_controller->fast_charging_vol);
	printf("57 . \tPeriodic Equalise Voltage \t\t%f\n", s_rectifier_controller->periodic_equilise_vol);
	printf("58 . \tManual Equalise Voltage \t\t%f\n", s_rectifier_controller->manual_equilise_vol);
	printf("59 . \tBattery Test Voltage \t\t\t%f\n", s_rectifier_controller->batt_test_volt);
	
	printf("\n\tNOTE : Value 32767 if sensor is not fitted or unassigned.\n");
	//printf("\n\tPress \"ENTER\" key to Continue to query\n\t");
	//getchar();
}
	
void parse_enatel_mains_monitor_data(const unsigned char *ibuf, 
						unsigned short numofreg, int command_num,
						struct enatel_mains_rectifier_controller *s_rectifier_controller)
{
	pr_debug("parse_enatel_mains_monitor_data : numofreg = %d, command_num = %d\n", 
			numofreg, command_num);
	int buf_count = 0, reg_count = 0, i = 0, j = 0;
	int data[numofreg/2];
	memset(data, '\0', sizeof(data));
	
	for (buf_count = 9; buf_count < numofreg; buf_count++, reg_count++ ) {
		data[reg_count] = ibuf[buf_count]<<8 | ibuf[++buf_count];
		pr_debug("data[%d] = %d\n", reg_count, data[reg_count]);
	}
	
	switch (command_num + 1) { 
		case 1 :
			s_rectifier_controller->bus_voltage = data[0] != 0x7fff ? data[0]/100 : data[0];
			s_rectifier_controller->num_of_rectifiers= data[1];
			s_rectifier_controller->rect_output_curr= data[2];
			s_rectifier_controller->load_curr= data[3];
			s_rectifier_controller->batt_curr = data[4];
			s_rectifier_controller->batt_capacity_remaining = data[5];
			s_rectifier_controller->battery_discharget_time_remaining = data[6];
			s_rectifier_controller->batt_temperature = data[7] != 0x7fff ? data[7]/10 : data[7];
			s_rectifier_controller->ambient_temperature = data[8] != 0x7fff ? data[8]/10 : data[8];
			s_rectifier_controller->num_of_12V_converters = data[9];
			s_rectifier_controller->_12V_converter_output_volt = data[10] != 0x7fff ? data[10]/100 : data[10];
			s_rectifier_controller->_12V_converter_output_curr = data[11];
			s_rectifier_controller->num_of_24V_converters = data[12];
			s_rectifier_controller->_24V_converter_output_volt = data[13] != 0x7fff ? data[13]/100 : data[13];
			s_rectifier_controller->_24V_converter_output_curr = data[14];
			s_rectifier_controller->num_of_48V_converters = data[15];
			s_rectifier_controller->_48V_converter_output_volt = data[16] != 0x7fff ? data[16]/100 : data[16];
			s_rectifier_controller->_48V_converter_output_curr = data[17];
			s_rectifier_controller->num_of_60V_converters = data[18];
			s_rectifier_controller->_60V_converter_output_volt = data[19] != 0x7fff ? data[19]/100 : data[19];
			s_rectifier_controller->_60V_converter_output_curr = data[20];
			s_rectifier_controller->number_of_inverters = data[21];
			s_rectifier_controller->inverter_output_ac_volt = data[22] != 0x7fff ? data[22]/10 : data[22];
			s_rectifier_controller->inverter_output_ac_curr = data[23] != 0x7fff ? data[23]/100 : data[23];
			s_rectifier_controller->fan_speed_1 = data[24] != 0x7fff ? data[24]/10 : data[24];
			s_rectifier_controller->fan_speed_2 = data[25] != 0x7fff ? data[25]/10 : data[25];
			
			
			for (reg_count = 26, i = 0 ; i < 16; i++, reg_count++)
				s_rectifier_controller->custom_variable[i] = data[reg_count];
				
			break;
		
		case 2 :
			for (i = 0, reg_count = 0; i < 3; i++)
				s_rectifier_controller->mains_monitor_1_ac_voltage[i] = 
							data[reg_count] != 0x7fff ? data[reg_count++]/10 : data[reg_count++];
			
			for (i = 0; i < 3; i++)	
				s_rectifier_controller->mains_monitor_1_ac_curr[i] = 
							data[reg_count] != 0x7fff ? data[reg_count++]/10 : data[reg_count++];
			
			s_rectifier_controller->mains_moninitor_1_frequency = data[reg_count++];
			
			for (i = 0; i < 3; i++)	
				s_rectifier_controller->mains_monitor_2_ac_voltage[i] = 
							data[reg_count] != 0x7fff ? data[reg_count++]/10 : data[reg_count++];
				
			for (i = 0; i < 3; i++)	
				s_rectifier_controller->mains_monitor_2_ac_curr[i] = 
							data[reg_count] != 0x7fff ? data[reg_count++]/10 : data[reg_count++];
			
			s_rectifier_controller->mains_moninitor_2_frequency = data[reg_count++];
			break; 
		
		case 3:		
			s_rectifier_controller->float_voltage = data[0] != 0x7fff ? data[0]/100 : data[0];
			s_rectifier_controller->fast_charging_vol = data[1] != 0x7fff ?data[1]/100 : data[1];
			s_rectifier_controller->periodic_equilise_vol = data[2] != 0x7fff ? data[2]/100 : data[2];
			s_rectifier_controller->manual_equilise_vol = data[3] != 0x7fff ?data[3]/100: data [3];
			s_rectifier_controller->batt_test_volt = data[4] != 0x7fff ? data[4]/100 : data[4];
			break;
	}
}

int enatel_data_read(const char *IPAddress, 
				char *tcp_iP_modbus_command, 
				unsigned char *ibuf,
				const int delay)
{
	struct sockaddr_in server;
	int ret;
	fd_set fds;
	struct timeval tv;
	/* establish connection to gateway on ASA standard port 502 */
	int s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	server.sin_family = AF_INET;
	server.sin_port = htons(MODBUS_TCP_SRANDARD_PORT); /* ASA standard port */
	server.sin_addr.s_addr = inet_addr(IPAddress);

	if ( connect(s, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0) {
		perror(" connect");
		close(s);
		return -1;
	}

	FD_ZERO(&fds);
	tv.tv_sec = delay;
	tv.tv_usec = 0;

	/* check ready to send */
	FD_SET(s, &fds);

	if (select(s+1, NULL, &fds, NULL, &tv) <= 0) {
		perror(" select");
		close(s);
		return -2;
	}
	
	pr_debug("printing tcp_iP_modbus_command : ");
	PrintHex(tcp_iP_modbus_command, 12);
	
	/* send request */
	if (send(s, tcp_iP_modbus_command, 12, 0) < 12) { 
		perror(" send");
		return -3;
	}
	/* wait for response */
	FD_SET(s, &fds);
	if ( select(s+1, &fds, NULL, NULL, &tv) <= 0 ) {
		printf(" no TCP response received\n");
		close(s);
		return -4;
	}

	/* receive and analyze response */
	ret = recv(s, ibuf, 1024, 0);
	pr_debug("printing recieved data from contoller : \n");
	PrintHex(ibuf, ret);
	
	/* close connection */
	close(s);
	return ret;
}

int query_enatel_mains_monitor_alarms( const char *IPAddress, 
					const int slaveid, 
					const int delay, int queryint, unsigned char *alarms)
{
	int ret = 0, command_num = 0;
	unsigned char tcp_iP_modbus_command[15];
	unsigned char ibuf[1024];
	unsigned short 	 startreg = 1, 
				numofreg = 176;
				
	memset( tcp_iP_modbus_command,'\0',sizeof(tcp_iP_modbus_command));
	memset(ibuf,'\0',sizeof(ibuf));
	
	pr_debug("query_enatel_mains_monitor_alarms : IPAddress = %s, slaveid = %d, delay = %d, queryint = %d\n",
			IPAddress, slaveid, delay, queryint);
			
	build_tcp_ip_modbus_command(tcp_iP_modbus_command, slaveid, 
						DESCRETE_INPUT_REGISTER_MAP_FUN, 
						DESCRETE_INPUT_REGISTER_START_REG+startreg-1, 
						numofreg, 
						command_num);
						
	ret  = enatel_data_read(IPAddress, tcp_iP_modbus_command, ibuf, delay);
	if (ret < 0 ) { 
		printf(" Failed to read data \n");
		return -1;
		
	} else if (ret < 9) {
		if (ret==0)
			printf("unexpected close of connection at remote end\n");
		else
			printf("response was too short - %d chars\n", ret);
		
		return -2;
		
	} else if (ibuf[7] & 0x80) {
		printf("MODBUS exception response - type %d\n", ibuf[8]);
		return -3;
			
	} else if (ibuf[8] > 0){ 
		parse_enatel_mains_monitor_alarms(ibuf, ret, alarms);
	}
	return 0;
}

int query_enatel_mains_monitor_data( const char *IPAddress, 
					const int slaveid, 
					struct enatel_mains_rectifier_controller *s_rectifier_controller,
					const int delay, int queryint)
{
	int ret = 0, command_num = 0;
	unsigned char tcp_iP_modbus_command[15];
	unsigned char ibuf[1024];
	unsigned short 	 startreg[] = {1, 101, 201}, 
				numofreg[] = {42, 15, 5}, 
				LengthField = 6;

	pr_debug("query_enatel_mains_monitor_data : IPAddress = %s, slaveid = %d, delay = %d, queryint = %d\n",
			IPAddress, slaveid, delay, queryint);
	

	for (; command_num < HOLDING_REG_COMMANDS_NUMS; command_num++) {
		pr_debug("querrying command_num = %d : startreg[%d] =%d, numofreg[%d]= %d\n", 
								command_num, command_num, startreg[command_num], 
								command_num, numofreg[command_num]);
		
		memset(ibuf,'\0',sizeof(ibuf));
		memset(tcp_iP_modbus_command, '\0', sizeof(tcp_iP_modbus_command));
		build_tcp_ip_modbus_command(tcp_iP_modbus_command, slaveid, HOLDING_REGISTER_MAP_FUN, 
							HOLDING_REG_BASE_REG_ADDRESS+startreg[command_num]-1, 
							numofreg[command_num], 
							command_num);
							
		ret  = enatel_data_read(IPAddress, tcp_iP_modbus_command, ibuf, delay);
		if (ret < 0 ) { 
			printf(" Failed to read data \n");
			return -1;
			
		} else if (ret < 9) {
			if (ret==0)
				printf("unexpected close of connection at remote end\n");
			else
				printf("response was too short - %d chars\n", ret);
			return -2;
		
		} else if (ibuf[7] & 0x80) {
			printf("MODBUS exception response - type %d\n", ibuf[8]);
			return -3;
				
		} else if (ret != (9+2*numofreg[command_num])) {
			printf("incorrect response size is %d expected %d\n",
					ret, (9+2*numofreg[command_num]));
							
		} else { 
			parse_enatel_mains_monitor_data(ibuf, ret, command_num, s_rectifier_controller);
		}
		
		os_sleep_in_ms(queryint);
	}
	return 0;
}


int main()
{
	int user_input = -1, slaveid, queryint, delay;
	int s;
	unsigned char data[1024];
	unsigned char alarms[180];
	char *temp = NULL;
	
	unsigned char IPAddress[20];
	struct enatel_mains_rectifier_controller s_rectifier_controller;

	const char *filename = "Config.txt";
	
	FILE *fp = NULL;

	memset( data,'\0',1024);
	memset( alarms,'\0',sizeof(alarms));

start_utility :
#if RESET
	system("reset");
	printf("\n\n\tEnatel_Mains_Monitor_Utility\n");
	printf("\n\t\tINVENDIS TECHNOLOGY\n");
#endif
	
	fp = fopen( filename, "r");
	if ( fp == NULL) {
		printf("KINDLY PUT THE Config.txt FILE IN THE CURRENT DIRECTORY AND RESTART THE APPLICATION\n\n");
		return -1;
		
	} else {
		printf ("\n\n\t------File %s Opened Succesfully------\n\n", filename);
		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL ) {
			temp++ ;
			slaveid = atoi(temp);
		}

		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL ) {
			temp++ ;
			strcpy(IPAddress, temp);
		}

		memset(data,0,sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL ) {
			temp++ ;
			delay  = atoi(temp);
		}

		memset(data,'\0',sizeof(data));
		fgets(data,sizeof(data), fp);
		if ( (temp = strchr ( data , '=' )) != NULL ) {
			temp++ ;
			queryint  = atoi(temp);
		}

		fclose (fp);
	}

	printf("\tslaveid=%d\n \tdelay=%d\n \tqueryint=%d\n \tIPAddress = %s\n", 
			slaveid, delay, queryint, IPAddress);
	os_sleep_in_ms(5*queryint);

#if RESET
	system("reset");
	printf("\n\n\t\tEnatel_Mains_Monitor_Utility\n");
	printf("\n\t\t\tINVENDIS TECHNOLOGY\n\n");
#endif

query_utility :
	printf(" Enter Your Option\n");
	printf("\t1 . Enatel Main Monitor Data \n\t2 . Enatel Main Monitor Alarms\n\t3 . Exit\n");
	printf(" Your Option : ");
	scanf("%d", &user_input);
	if (user_input > 3 || user_input < 1) {
		printf(" Please Enter The Correct Option\n");
		sleep(2);

#if RESET
	system("reset");
	printf("\n\n\t\tEnatel_Mains_Monitor_Utility\n");
	printf("\n\t\t\tINVENDIS TECHNOLOGY\n\n");
#endif		
		goto query_utility;	
	}
	
	sleep(2);
	
	switch (user_input) {
		
		case 1 : 
			if (query_enatel_mains_monitor_data(IPAddress, slaveid, 
							&s_rectifier_controller, delay, queryint) < 0) {
#if RESET
				system("reset");
				printf("\n\n\t\tEnatel_Mains_Monitor_Utility\n");
				printf("\n\t\t\tINVENDIS TECHNOLOGY\n");
#endif			
				printf("\n CAUTION : FAILED TO Get Main Monitor Data \n\t   Please check the connection or Configuration\n\n");
				goto error_checking;
			}
#if RESET
			system("reset");
			printf("\n\n\t\tEnatel_Mains_Monitor_Utility\n");
			printf("\n\t\t\tINVENDIS TECHNOLOGY\n");
#endif			
			display_enatel_mains_monitor_data(&s_rectifier_controller);
			memset(&s_rectifier_controller, '\0', sizeof(struct enatel_mains_rectifier_controller));
			printf("\n\n");
		break;
		
		case 2 : 
			if (query_enatel_mains_monitor_alarms(IPAddress, slaveid, 
							delay, queryint, alarms) < 0 ) {
#if RESET
				system("reset");
				printf("\n\n\t\tEnatel_Mains_Monitor_Utility\n");
				printf("\n\t\t\tINVENDIS TECHNOLOGY\n");
#endif			
				printf("\n CAUTION : FAILED TO Get Main Monitor ALARMS \n\t   Please check the connection or Configuration\n\n");
				goto error_checking;
			}		
#if RESET
			system("reset");
			printf("\n\n\t\tEnatel_Mains_Monitor_Utility\n");
			printf("\n\t\t\tINVENDIS TECHNOLOGY\n");
#endif	
			display_enatel_mains_monitor_alarms(alarms);
			memset(alarms, '\0', sizeof(alarms));
		break;
		
		case 3 :
			return 0;
		
		default:
			printf(" Wrong Option You Have Entered and Exiting \n");
			return 0;
			break;
	}
error_checking:	
	goto query_utility;
	return 0;
}
