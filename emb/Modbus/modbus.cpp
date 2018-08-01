#include <Modbus/modbus.h>

#define REG_COIL_START				0x0001
#define REG_COIL_NREGS				32
/*static*/ USHORT usRegCoilStart = REG_COIL_START;
/*static*/ BOOL   usRegCoilBuf[REG_COIL_NREGS] = {0};

#define REG_DISCRETE_START			0x1001
#define REG_DISCRETE_NREGS			24
/*static*/ USHORT usRegDiscreteStart = REG_DISCRETE_START;
/*static*/ BOOL   usRegDiscreteBuf[REG_DISCRETE_NREGS] = {0};

#define REG_HOLDING_START			0x4001
#define REG_HOLDING_NREGS			3
/*static*/ USHORT usRegHoldingStart = REG_HOLDING_START;
/*static*/ USHORT usRegHoldingBuf[REG_HOLDING_NREGS] = {0};


#define REG_INPUT_START			0x3001
#define REG_INPUT_NREGS			9
/*static*/ USHORT usRegInputStart = REG_INPUT_START;
/*static*/ USHORT usRegInputBuf[REG_INPUT_NREGS] = {0};




Modbus::Modbus() {


	eMBErrorCode status;

	status = eMBInit(eMBMode::MB_RTU, MODBUS_ADDRESS, 0, MODBUS_BAUDRATE, MB_PAR_NONE);
	if(status != eMBErrorCode::MB_ENOERR) {
			__asm("nop");
	}

	status = eMBEnable(  );
	if(status != eMBErrorCode::MB_ENOERR) {
				__asm("nop");
	}

}

/*bool DataTransceiver::init() {


	return true;

}*/



void Modbus::poll()
{
	eMBErrorCode status;
	status = eMBPoll();
	if(status != eMBErrorCode::MB_ENOERR) {
		__asm("nop");
	}
	__asm("nop");

}

void Modbus::set_register(RegisterType type, uint8_t address, uint16_t value)
{
	if ( (type == COIL) && (address < REG_COIL_NREGS) ) {
		usRegCoilBuf[address] = value;
	}
	else if ( (type == DISCRETE) && (address < REG_DISCRETE_NREGS) ) {
		usRegDiscreteBuf[address] = value;
	}
	else if((type == HOLDING) && (address < REG_HOLDING_NREGS)) {
		usRegHoldingBuf[address] = value;
	}
	else if((type == INPUT) && (address < REG_INPUT_NREGS)) {
		usRegInputBuf[address] = value;
	}
}

/*auto Modbus::get_register(RegisterType type, uint8_t address, uint16_t &value) -> Error {

}*/

Modbus::Error Modbus::get_register(RegisterType type, uint8_t address, uint16_t &value)
{

	if ( (type == COIL) && (address < REG_COIL_NREGS) ) {
		value = usRegCoilBuf[address];
		return Error::MB_ENOERR;
	}
	else if ( (type == DISCRETE) && (address < REG_DISCRETE_NREGS) ) {
		value = usRegDiscreteBuf[address];
		return Error::MB_ENOERR;
	}
	else if((type == HOLDING)  && (address < REG_HOLDING_NREGS)) {
		value = usRegHoldingBuf[address];
		return Error::MB_ENOERR;
	}
	else if((type == INPUT)  && (address < REG_INPUT_NREGS)) {
		value = usRegInputBuf[address];
		return Error::MB_ENOERR;
	}
	return Error::MB_ENOERR;
}


/**************************************************************/
/**********************  USER CODE ****************************/
/**************************************************************/

eMBErrorCode    eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress,
                               USHORT usNRegs ) {
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;

	if( ( usAddress >= REG_INPUT_START )
		&& ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
	{
		iRegIndex = ( int )( usAddress - usRegInputStart );
		while( usNRegs > 0 )
		{
			*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
			*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
			iRegIndex++;
			usNRegs--;
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

eMBErrorCode    eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress,
                                 USHORT usNRegs, eMBRegisterMode eMode ) {
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;

	if( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
	{
		iRegIndex = ( int )( usAddress - usRegHoldingStart );
		switch ( eMode )
		{
		case MB_REG_READ:
			while( usNRegs > 0 )
			{
				*pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
				*pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
				iRegIndex++;
				usNRegs--;
			}
			break;

		case MB_REG_WRITE:
			while( usNRegs > 0 )
			{
				usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
				usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
				iRegIndex++;
				usNRegs--;
			}
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

eMBErrorCode    eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress,
                               USHORT usNCoils, eMBRegisterMode eMode ) {
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;

	if( ( usAddress >= REG_COIL_START ) && ( usAddress + usNCoils <= REG_COIL_START + REG_COIL_NREGS ) )
	{
		iRegIndex = ( int )( usAddress - usRegCoilStart );
		switch ( eMode )
		{
		case MB_REG_READ:
			while( usNCoils > 0 )
			{
				*pucRegBuffer++ = (unsigned char)(usRegCoilBuf[iRegIndex]);
				iRegIndex++;
				usNCoils--;
			}
			break;

		case MB_REG_WRITE:
			while( usNCoils > 0 )
			{
				usRegCoilBuf[iRegIndex] = *pucRegBuffer++;
				iRegIndex++;
				usNCoils--;
			}
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

eMBErrorCode    eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress,
                                  USHORT usNDiscrete ) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_DISCRETE_START )
        && ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegDiscreteStart );
        while( usNDiscrete > 0 )
        {
        	*pucRegBuffer++ = (unsigned char)(usRegDiscreteBuf[iRegIndex]);
            //*pucRegBuffer++ = ( unsigned char )( usRegDiscreteBuf[iRegIndex] >> 8 );
            //*pucRegBuffer++ = ( unsigned char )( usRegDiscreteBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNDiscrete--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}










