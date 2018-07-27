/*
 * data_transceiver.h
 *
 *  Created on: 26 ���. 2018 �.
 *      Author: ������
 */

#ifndef MODBUS_MODBUS_H_
#define MODBUS_MODBUS_H_

#include <Modbus/registers.h>
/*#include "modbus.h"
#include "mb.h"*/


class DataTransceiver {
public:
	DataTransceiver();
	//bool init();
	void poll();
	void set_flag(RegisterType type, uint8_t address, bool value);
	bool get_flag(RegisterType type, uint8_t address);
	void set_register(RegisterType type, uint8_t address, uint16_t value);
	uint16_t get_register(RegisterType type, uint8_t address);

};



#endif /* MODBUS_MODBUS_H_ */
