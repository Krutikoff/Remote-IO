/*
 * modbus.h
 *
 *  Created on: 26 июл. 2018 г.
 *      Author: Максим
 */



/*	Platform STM32f103C8T6	*/

#ifndef MODBUS_CONFIG_MB_H_
#define MODBUS_CONFIG_MB_H_


#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>


#define MODBUS_UART				USART1
#define MODBUS_IRQ              NVIC_USART1_IRQ
#define MODBUS_IRQ_HANDLER      usart1_isr


#define MODBUS_STOPBITS			USART_STOPBITS_1
#define MODBUS_FLOWCONTROL      USART_FLOWCONTROL_NONE
#define MODBUS_MODE             USART_MODE_TX_RX
#define MODBUS_DATABITS         8

#define MODBUS_PORT              	GPIOA
#define MODBUS_DE_PIN            	GPIO8
#define MODBUS_RX_PIN				GPIO10
#define MODBUS_TX_PIN				GPIO9
#define MODBUS_ADDRESS				0x01
#define MODBUS_BAUDRATE          	115200


#endif /* MODBUS_CONFIG_MB_H_ */
