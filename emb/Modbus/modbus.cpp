#include "Modbus/config_mb.h"
#include <Modbus/modbus.h>

DataTransceiver::DataTransceiver() {

	gpio_set_mode(MODBUS_PORT, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, MODBUS_TX_PIN);
	gpio_set_mode(MODBUS_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, MODBUS_RX_PIN);

	eMBErrorCode    eStatus;


	eStatus = eMBInit(MB_RTU, MODBUS_ADDRESS, 0, MODBUS_BAUDRATE, MB_PAR_NONE);
	if(eStatus != MB_ENOERR) return;


	eStatus = eMBEnable(  );
	if(eStatus != MB_ENOERR) return;

}

/*bool DataTransceiver::init() {


	return true;

}*/



void DataTransceiver::poll()
{
	(void)eMBPoll();
}

void DataTransceiver::set_register(RegisterType type, uint8_t address, uint16_t value)
{
	if ( (type == COIL) && (address < REG_COIL_NREGS) ) {
		usRegCoilBuf[address] = value;
	}
	else if ( (type == DISCRETE) && (address < REG_DISCRETE_NREGS) ) {
		usRegDiscreteBuf[address] = value;
	}
	else if((type == HOLDING) (address < REG_HOLDING_NREGS)) {
		usRegHoldingBuf[address] = value;
	}
	else if((type == INPUT) (address < REG_INPUT_NREGS)) {
		usRegInputBuf[address] = value;
	}
}

uint16_t DataTransceiver::get_register(RegisterType type, uint8_t address)
{
	if ( (type == COIL) && (address < REG_COIL_NREGS) ) {
		return usRegCoilBuf[address];
	}
	else if ( (type == DISCRETE) && (address < REG_DISCRETE_NREGS) ) {
		return usRegDiscreteBuf[address];
	}
	else if((type == HOLDING) (address < REG_HOLDING_NREGS)) {
		return usRegHoldingBuf[address];
	}
	else if((type == INPUT) (address < REG_INPUT_NREGS)) {
		return usRegInputBuf[address];
	}
	return 0;
}









