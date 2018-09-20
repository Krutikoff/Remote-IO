#pragma once

#include "config_modbus.h"
#include <mb.h>
#include <Module/encoder.h>

class Modbus
{
 public:
    /* Registers protocol Modbus */
    enum class RegisterType
    {
        COIL,
        DISCRETE,
        HOLDING,
        INPUT,
        ERROR
    };

    static constexpr uint32_t REG_COIL_START = 0x0001;
    static constexpr uint32_t REG_COIL_NREGS = 32;

    static constexpr uint32_t REG_DISCRETE_START = 0x1001;
    static constexpr uint32_t REG_DISCRETE_NREGS = 24;

    static constexpr uint32_t REG_HOLDING_START = 0x4001;
    static constexpr uint32_t REG_HOLDING_NREGS = 4;

    static constexpr uint32_t REG_INPUT_START = 0x3001;
    static constexpr uint32_t REG_INPUT_NREGS = 11;

    using Error = eMBErrorCode;
    uint8_t modbus_address_device;

    static Modbus& instance()
    {
        static Modbus instance;
        return instance;
    }

    bool poll();

    RegisterType _get_type(uint32_t address);
    template<typename T>
    T* get_iterator(uint32_t address);

 private:
    static USHORT usRegCoilStart;
    static BOOL usRegCoilBuf[REG_COIL_NREGS];

    static USHORT usRegDiscreteStart;
    static BOOL usRegDiscreteBuf[REG_DISCRETE_NREGS];

    static USHORT usRegHoldingStart;
    static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];  // USHORT

    static USHORT usRegInputStart;
    static USHORT usRegInputBuf[REG_INPUT_NREGS];

    static constexpr uint32_t LatchClearReg = 0x100;

    Modbus();
    Modbus(const Modbus&) = default;
    Modbus& operator=(const Modbus&);

    friend eMBErrorCode eMBRegInputCB(UCHAR* pucRegBuffer,
                                      USHORT usAddress,
                                      USHORT usNRegs);
    friend eMBErrorCode eMBRegHoldingCB(UCHAR* pucRegBuffer,
                                        USHORT usAddress,
                                        USHORT usNRegs,
                                        eMBRegisterMode eMode);
    friend eMBErrorCode eMBRegCoilsCB(UCHAR* pucRegBuffer,
                                      USHORT usAddress,
                                      USHORT usNCoils,
                                      eMBRegisterMode eMode);
    friend eMBErrorCode eMBRegDiscreteCB(UCHAR* pucRegBuffer,
                                         USHORT usAddress,
                                         USHORT usNDiscrete);
};

template<typename T>
T* Modbus::get_iterator(uint32_t address)
{

    RegisterType type = _get_type(address);

    if (type == RegisterType::COIL &&
        (address < REG_COIL_NREGS + REG_COIL_START)) {
        return reinterpret_cast<T*>(usRegCoilBuf + address - REG_COIL_START);
    }
    else if (type == RegisterType::DISCRETE &&
             (address < REG_DISCRETE_NREGS + REG_DISCRETE_START)) {
        return reinterpret_cast<T*>(usRegDiscreteBuf + address -
                                    REG_DISCRETE_START);
    }
    else if (type == RegisterType::HOLDING &&
             (address < REG_HOLDING_NREGS + REG_HOLDING_START)) {
        return reinterpret_cast<T*>(usRegHoldingBuf + address -
                                    REG_HOLDING_START);
    }
    else if (type == RegisterType::INPUT &&
             (address < REG_INPUT_NREGS + REG_INPUT_START)) {
        return reinterpret_cast<T*>(usRegInputBuf + address - REG_INPUT_START);
    }
    else
        return nullptr;
}
