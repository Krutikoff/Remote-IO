#pragma once

#include <Modbus/registers.h>
#include <Modbus/config_modbus.h>
/*#include "modbus.h"
#include "mb.h"*/


class Modbus {
public:
	using Error = eMBErrorCode;
	Modbus();
	//bool init();
	void poll();
	void set_flag(RegisterType type, uint8_t address, bool value);
	bool get_flag(RegisterType type, uint8_t address);
	void set_register(RegisterType type, uint8_t address, uint16_t value);
	Error get_register(RegisterType type, uint8_t address, uint16_t &value);

};




