#ifndef ENATEL_RECTIFIER_UTILITY_H__
#define ENATEL_RECTIFIER_UTILITY_H__

#ifdef DEBUG_PRINTF 
	#define pr_debug	printf
#else 
	#define pr_debug
	#define RESET	1
#endif

#define MODBUS_TCP_SRANDARD_PORT	502
#define MODBUS_TCP_DATA_FIELD	6
#define HOLDING_REG_COMMANDS_NUMS	3
#define HOLDING_REG_BASE_REG_ADDRESS	40000

#define DESCRETE_INPUT_REGISTER_MAP_FUN	2
#define HOLDING_REGISTER_MAP_FUN	3
#define DESCRETE_INPUT_REGISTER_START_REG	10000
#define DESCRETE_INPUT_REGISTER_REG_NUM	176

struct enatel_mains_rectifier_controller {
	float bus_voltage;
	int num_of_rectifiers;
	int rect_output_curr;
	int load_curr;
	int batt_curr;
	int batt_capacity_remaining;
	int battery_discharget_time_remaining;
	float batt_temperature;
	float ambient_temperature;
	int num_of_12V_converters;
	float _12V_converter_output_volt;
	int _12V_converter_output_curr;
	int num_of_24V_converters;
	float _24V_converter_output_volt;
	int _24V_converter_output_curr;
	int num_of_48V_converters;
	float _48V_converter_output_volt;
	int _48V_converter_output_curr;
	int num_of_60V_converters;
	float _60V_converter_output_volt;
	int _60V_converter_output_curr;
	int number_of_inverters;
	float inverter_output_ac_volt;
	float inverter_output_ac_curr;
	float fan_speed_1;
	float fan_speed_2;
	int  custom_variable[16];
	float mains_monitor_1_ac_voltage[3];
	float mains_monitor_1_ac_curr[3];
	int  mains_moninitor_1_frequency;
	float mains_monitor_2_ac_voltage[3];
	float mains_monitor_2_ac_curr[3];
	int  mains_moninitor_2_frequency;
	float float_voltage;
	float fast_charging_vol;
	float periodic_equilise_vol;
	float manual_equilise_vol;
	float batt_test_volt;
};

#endif


