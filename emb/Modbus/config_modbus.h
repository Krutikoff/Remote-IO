#pragma once

#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/cm3/nvic.h>

#define MODBUS_UART				USART1 // <libopencm3/stm32/common/usart_common_f124.h>
#define MODBUS_IRQ              NVIC_USART1_IRQ // libopencm3/stm32/f1/nvic.h
#define MODBUS_IRQ_HANDLER      usart1_isr // libopencm3/stm32/f1/nvic.h

#define MODBUS_STOPBITS			USART_STOPBITS_1 // <libopencm3/stm32/common/usart_common_all.h>
#define MODBUS_FLOWCONTROL      USART_FLOWCONTROL_NONE // <libopencm3/stm32/common/usart_common_all.h>
#define MODBUS_MODE             USART_MODE_TX_RX // <libopencm3/stm32/common/usart_common_all.h>
#define MODBUS_DATABITS         8

#define MODBUS_PORT              	GPIOA
#define MODBUS_DE_PORT				GPIOB
#define MODBUS_DE_PIN            	GPIO0
#define MODBUS_RX_PIN				GPIO_USART1_RX
#define MODBUS_TX_PIN				GPIO_USART1_TX
#define MODBUS_ADDRESS				0x01
#define MODBUS_BAUDRATE          	115200



