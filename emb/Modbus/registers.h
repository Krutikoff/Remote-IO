#ifndef MODBUS_REGISTERS_H_
#define MODBUS_REGISTERS_H_

#include <Modbus/config_mb.h>
#include <mb.h>




#define REG_COIL_START				0x0001
#define REG_COIL_NREGS				32
static USHORT usRegCoilStart = REG_COIL_START;
static BOOL   usRegCoilBuf[REG_COIL_NREGS] = {0};

#define REG_DISCRETE_START			0x1001
#define REG_DISCRETE_NREGS			24
static USHORT usRegDiscreteStart = REG_DISCRETE_START;
static BOOL   usRegDiscreteBuf[REG_DISCRETE_NREGS] = {0};

#define REG_HOLDING_START			0x4001
#define REG_HOLDING_NREGS			3
static USHORT usRegHoldingStart = REG_HOLDING_START;
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS] = {0};


#define REG_INPUT_START			0x3001
#define REG_INPUT_NREGS			9
static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS] = {0};

//#include <stdint.h>


/* Registers protocol Modbus */
typedef enum {
	COIL,
	DISCRETE,
	HOLDING,
	INPUT

} RegisterType;

typedef enum : uint8_t {
	GPIO_SET_BIT0,
	GPIO_SET_BIT1,
	GPIO_SET_BIT2,
	GPIO_SET_BIT3,
	GPIO_SET_BIT4,
	GPIO_SET_BIT5,
	GPIO_SET_BIT6,
	GPIO_SET_BIT7,
	GPIO_WRITE_BIT0,
	GPIO_WRITE_BIT1,
	GPIO_WRITE_BIT2,
	GPIO_WRITE_BIT3,
	GPIO_WRITE_BIT4,
	GPIO_WRITE_BIT5,
	GPIO_WRITE_BIT6,
	GPIO_WRITE_BIT7,
	LATCH_CLEAR,
	COUNTER0_CLEAR,
	COUNTER1_CLEAR,
	COUNTER2_CLEAR,
	COUNTER3_CLEAR,
	COUNTER4_CLEAR,
	COUNTER5_CLEAR,
	COUNTER6_CLEAR,
	COUNTER7_CLEAR,
	I2C_ENABLE,
	I2C_START,
	I2C_STOP,
	I2C_START_CONDITION,
	I2C_ADDR_SEND,
	I2C_RXNE,
	I2C_TXE
}Coil;

typedef enum : uint8_t {
	GPIO_READ_BIT0,
	GPIO_READ_BIT1,
	GPIO_READ_BIT2,
	GPIO_READ_BIT3,
	GPIO_READ_BIT4,
	GPIO_READ_BIT5,
	GPIO_READ_BIT6,
	GPIO_READ_BIT7,
	LATCH0_LOW,
	LATCH1_LOW,
	LATCH2_LOW,
	LATCH3_LOW,
	LATCH4_LOW,
	LATCH5_LOW,
	LATCH6_LOW,
	LATCH7_LOW,
	LATCH0_HIGH,
	LATCH1_HIGH,
	LATCH2_HIGH,
	LATCH3_HIGH,
	LATCH4_HIGH,
	LATCH5_HIGH,
	LATCH6_HIGH,
	LATCH7_HIGH
}Discrete;


typedef enum : uint8_t {
	COUNTER0_VALUE,
	COUNTER1_VALUE,
	COUNTER2_VALUE,
	COUNTER3_VALUE,
	COUNTER4_VALUE,
	COUNTER5_VALUE,
	COUNTER6_VALUE,
	COUNTER7_VALUE,
	I2C_RX_DATA
}Input;

typedef enum : uint8_t {
	UART_BAUDRATE_0,
	UART_BAUDRATE_1,
	I2C_TX_DATA
}Holding;




#endif /* MODBUS_REGISTERS_H_ */
