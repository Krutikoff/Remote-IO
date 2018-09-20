#include "modbus.h"
#include <algorithm>

USHORT Modbus::usRegCoilStart = REG_COIL_START;
BOOL Modbus::usRegCoilBuf[REG_COIL_NREGS] = { 0 };

USHORT Modbus::usRegDiscreteStart = REG_DISCRETE_START;
BOOL Modbus::usRegDiscreteBuf[REG_DISCRETE_NREGS] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
                                                      1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

USHORT Modbus::usRegHoldingStart = REG_HOLDING_START;
USHORT Modbus::usRegHoldingBuf[REG_HOLDING_NREGS] = { 0 }; //USHORT

USHORT Modbus::usRegInputStart = REG_INPUT_START;
USHORT Modbus::usRegInputBuf[REG_INPUT_NREGS] = { 0 };

Modbus *_modbus_ptr;

Modbus::Modbus():modbus_address_device()
{
    _modbus_ptr = this;

    eMBErrorCode status;

    EncoderModule& encoder_module = EncoderModule::instance();
    modbus_address_device = encoder_module.get_address_device();

    status = eMBInit(eMBMode::MB_RTU, modbus_address_device, 0, MODBUS_BAUDRATE,
                     MB_PAR_NONE);
    if (status != eMBErrorCode::MB_ENOERR) {
        __asm("nop");
    }

    status = eMBEnable();
    if (status != eMBErrorCode::MB_ENOERR) {
        __asm("nop");
    }

}

bool Modbus::poll()
{
    eMBErrorCode status;
    status = eMBPoll();
    if (status != eMBErrorCode::MB_ENOERR) {
        __asm("nop");
        return false;
    }
    else {
        __asm("nop");
        return true;

    }

}

Modbus::RegisterType Modbus::_get_type(uint32_t address)
{
    RegisterType type;

    if (address >= REG_COIL_START && address <= REG_COIL_START + REG_COIL_NREGS) {
        return type = RegisterType::COIL;
    }
    else if (address >= REG_DISCRETE_START
        && address <= REG_DISCRETE_START + REG_DISCRETE_NREGS) {
        return type = RegisterType::DISCRETE;
    }
    else if (address >= REG_HOLDING_START
        && address <= REG_HOLDING_START + REG_HOLDING_NREGS) {
        return type = RegisterType::HOLDING;
    }
    else if (address >= REG_INPUT_START && address <= REG_INPUT_START + REG_INPUT_NREGS) {
        return type = RegisterType::INPUT;
    }
    else
        return RegisterType::ERROR;
}

/**************************************************************/
/**********************  MODBUS FUNCTION CODE ****************************/
/**************************************************************/

eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;

    Modbus& modbus = Modbus::instance();
    if ((usAddress >= modbus.REG_INPUT_START)
        && (usAddress + usNRegs <= modbus.REG_INPUT_START + modbus.REG_INPUT_NREGS)) {
        iRegIndex = (int) (usAddress - modbus.usRegInputStart);
        while (usNRegs > 0) {
            *pucRegBuffer++ = (unsigned char) (modbus.usRegInputBuf[iRegIndex] >> 8);
            *pucRegBuffer++ = (unsigned char) (modbus.usRegInputBuf[iRegIndex] & 0xFF);
            iRegIndex++;
            usNRegs--;
        }
    }
    else {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                             eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;

    Modbus& modbus = Modbus::instance();
    if ((usAddress >= modbus.REG_HOLDING_START)
        && (usAddress + usNRegs <= modbus.REG_HOLDING_START + modbus.REG_HOLDING_NREGS)) {
        iRegIndex = (int) (usAddress - modbus.usRegHoldingStart);
        switch (eMode)
        {
            case MB_REG_READ:
                while (usNRegs > 0) {
                    *pucRegBuffer++ = (unsigned char) (modbus.usRegHoldingBuf[iRegIndex]
                        >> 8);
                    *pucRegBuffer++ = (unsigned char) (modbus.usRegHoldingBuf[iRegIndex]
                        & 0xFF);
                    iRegIndex++;
                    usNRegs--;
                }
            break;

            case MB_REG_WRITE:
                while (usNRegs > 0) {
                    modbus.usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                    modbus.usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                    iRegIndex++;
                    usNRegs--;
                }
        }
    }
    else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
                           eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;

    Modbus& modbus = Modbus::instance();
    if ((usAddress >= modbus.REG_COIL_START)
        && (usAddress + usNCoils <= modbus.REG_COIL_START + modbus.REG_COIL_NREGS)) {
        iRegIndex = (int) (usAddress - modbus.usRegCoilStart);
        switch (eMode)
        {
            case MB_REG_READ:
                while (usNCoils > 0) {
                    *pucRegBuffer++ = (unsigned char) (modbus.usRegCoilBuf[iRegIndex]);
                    iRegIndex++;
                    usNCoils--;
                }
            break;

            case MB_REG_WRITE:
                while (usNCoils > 0) {
                    modbus.usRegCoilBuf[iRegIndex] = *pucRegBuffer++;
                    iRegIndex++;
                    usNCoils--;
                }
        }
    }
    else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;

    Modbus& modbus = Modbus::instance();
    if ((usAddress >= modbus.REG_DISCRETE_START)
        && (usAddress + usNDiscrete
            <= modbus.REG_DISCRETE_START + modbus.REG_DISCRETE_NREGS)) {
        iRegIndex = (int) (usAddress - modbus.usRegDiscreteStart);
        while (usNDiscrete > 0) {
            *pucRegBuffer++ = (unsigned char) (modbus.usRegDiscreteBuf[iRegIndex]);
            //*pucRegBuffer++ = ( unsigned char )( usRegDiscreteBuf[iRegIndex] >> 8 );
            //*pucRegBuffer++ = ( unsigned char )( usRegDiscreteBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNDiscrete--;
        }
    }
    else {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

