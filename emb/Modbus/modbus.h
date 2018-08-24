#pragma once


#include <Modbus/config_modbus.h>
#include <mb.h>

#define FALLING 0
#define RISING 1

#define WRITE_MODE 1
#define READ_MODE 0

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
    static constexpr uint32_t REG_HOLDING_NREGS = 3;

    static constexpr uint32_t REG_INPUT_START = 0x3001;
    static constexpr uint32_t REG_INPUT_NREGS = 9;

    using Error = eMBErrorCode;
    uint8_t modbus_address_device;
    bool config_write[8] = { false };
    bool config_read[8] = { true }; // Set read mode default
    uint32_t baudrate = 0;

//    static uint8_t* coils_it_gpio_set;
//    static uint8_t* coils_it_gpio_write;
    static uint8_t* coils_it_latch_clear;
    static uint8_t* coils_it_counter_clear;

    static uint8_t* discrete_it_read;
    static uint8_t* discrete_it_latch_low;
    static uint8_t* discrete_it_latch_high;

    static uint16_t* input_it_counter;

    static uint32_t* holding_it_baudrate; // 16 * 2 = 32

    //uint8_t* discrete_it_read = get_iterator <uint8_t>(0x1001);

    static Modbus& instance()
    {
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
    template<typename T> T* get_iterator(uint32_t address);

    void mb_gpio_set(uint8_t* coil_data_set);
    void mb_gpio_write(uint8_t* coil_data_write);
    void mb_latch_low(uint8_t* input_latch_low);
    void mb_latch_high(uint8_t* input_latch_high);
    void mb_latch_clear();
    void mb_counter_clear();
    void mb_baudrate_set();

private:

    static USHORT usRegCoilStart;
    static BOOL usRegCoilBuf[REG_COIL_NREGS];

    static USHORT usRegDiscreteStart;
    static BOOL usRegDiscreteBuf[REG_DISCRETE_NREGS];

    static USHORT usRegHoldingStart;
    static USHORT usRegHoldingBuf[REG_HOLDING_NREGS]; //USHORT

    static USHORT usRegInputStart;
    static USHORT usRegInputBuf[REG_INPUT_NREGS];

    static constexpr uint32_t LatchClearReg = 0x100;

    Modbus();
    Modbus(const Modbus&) = default;
    Modbus& operator=(const Modbus&);

    friend eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                      USHORT usNRegs);
    friend eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                        USHORT usNRegs, eMBRegisterMode eMode);
    friend eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                      USHORT usNCoils, eMBRegisterMode eMode);
    friend eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                         USHORT usNDiscrete);
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

template<typename T>
T* Modbus::get_iterator(uint32_t address)
{

    RegisterType type = _get_type(address);

    if (type == RegisterType::COIL && (address < REG_COIL_NREGS + REG_COIL_START)) {
        return reinterpret_cast<T*>(usRegCoilBuf + address - REG_COIL_START);

    }
    else if (type == RegisterType::DISCRETE
        && (address < REG_DISCRETE_NREGS + REG_DISCRETE_START)) {
        return reinterpret_cast<T*>(usRegDiscreteBuf + address - REG_DISCRETE_START);
    }
    else if (type == RegisterType::HOLDING
        && (address < REG_HOLDING_NREGS + REG_HOLDING_START)) {
        return reinterpret_cast<T*>(usRegHoldingBuf + address - REG_HOLDING_START);
    }
    else if (type == RegisterType::INPUT
        && (address < REG_INPUT_NREGS + REG_INPUT_START)) {
        return reinterpret_cast<T*>(usRegInputBuf + address - REG_INPUT_START);
    }
    else
        return nullptr;
}

