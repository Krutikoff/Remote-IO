#pragma once

#include <Modbus/registers.h>
#include <Modbus/config_modbus.h>
/*#include "modbus.h"
#include "mb.h"*/


class Modbus {

public:

	using Error = eMBErrorCode;

	static Modbus& instance(){
		static Modbus instance;
		return instance;
	}
	//bool init();
	void poll();
	void set_flag(RegisterType type, uint8_t address, bool value);
	bool get_flag(RegisterType type, uint8_t address);
	void set_register(RegisterType type, uint32_t address, uint16_t value);
	Error get_register(RegisterType type, uint32_t address, uint16_t &value);


	RegisterType _get_type(uint32_t address);
	template <typename T> T* get_iterator( uint32_t address);


private:
	static constexpr uint32_t REG_COIL_START = 0x0001;
	static constexpr uint32_t REG_COIL_NREGS = 32;
	static USHORT usRegCoilStart;
	static BOOL   usRegCoilBuf[REG_COIL_NREGS];

	static constexpr uint32_t REG_DISCRETE_START = 0x1001;
	static constexpr uint32_t REG_DISCRETE_NREGS = 24;
	static USHORT usRegDiscreteStart;
	static BOOL   usRegDiscreteBuf[REG_DISCRETE_NREGS];

	static constexpr uint32_t REG_HOLDING_START = 0x4001;
	static constexpr uint32_t REG_HOLDING_NREGS = 3;
	static USHORT usRegHoldingStart;
	static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

	static constexpr uint32_t REG_INPUT_START = 0x3001;
	static constexpr uint32_t  REG_INPUT_NREGS = 9;
	static USHORT usRegInputStart;
	static USHORT usRegInputBuf[REG_INPUT_NREGS];

	Modbus();
	Modbus(const Modbus&) = default;
	Modbus& operator=(const Modbus&);

	friend eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress,
	                               USHORT usNRegs );
	friend eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress,
	                                 USHORT usNRegs, eMBRegisterMode eMode );
	friend eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress,
	                               USHORT usNCoils, eMBRegisterMode eMode );
	friend eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress,
	                                  USHORT usNDiscrete );
};

//auto Modbus::get_iterator(RegisterType type, uint32_t address) {
//
//
//	if(type == RegisterType::COIL && (address < REG_HOLDING_NREGS)) {
//		return  usRegCoilBuf + address;
//
//	}
//	else if(type == RegisterType::DISCRETE && (address < REG_HOLDING_NREGS)) {
//		return  usRegDiscreteBuf + address;
//	}
//	else if(type == RegisterType::HOLDING && (address < REG_HOLDING_NREGS)) {
//		return  usRegHoldingBuf + address;
//	}
//	else if(type == RegisterType::INPUT && (address < REG_HOLDING_NREGS)) {
//		return  usRegInputBuf + address;
//	}
//	else
//		return nullptr;
//}







template <typename T>
T* Modbus::get_iterator(uint32_t address){

	RegisterType type = _get_type(address);

	if(type == RegisterType::COIL && (address < REG_COIL_NREGS + REG_COIL_START)) {
		return  reinterpret_cast<T*>(usRegCoilBuf + address - REG_COIL_START);

	}
	else if(type == RegisterType::DISCRETE && (address < REG_DISCRETE_NREGS + REG_DISCRETE_START)) {
		return   reinterpret_cast<T*>(usRegDiscreteBuf + address);
	}
	else if(type == RegisterType::HOLDING && (address < REG_HOLDING_NREGS + REG_HOLDING_START)) {
		return  reinterpret_cast<T*>(usRegHoldingBuf + address);
	}
	else if(type == RegisterType::INPUT && (address < REG_INPUT_NREGS+ REG_INPUT_START)) {
		return  reinterpret_cast<T*>(usRegInputBuf + address);
	}
	else
		return nullptr;
}



