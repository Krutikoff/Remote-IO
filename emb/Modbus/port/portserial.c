/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "../../Modbus/port/port.h"

#include "mb.h"
#include "mbport.h"

/* USER CODE*/
#include "libopencm3/stm32/f1/nvic.h"
#include <libopencm3/stm32/rcc.h>
#include <Modbus/config_modbus.h>
#include <libopencm3/stm32/gpio.h>
/* END*/

/* ----------------------- static functions ---------------------------------*/
/*static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );*/

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */

	if(xRxEnable && !xTxEnable)
	{
		while(!usart_get_flag( MODBUS_UART, USART_SR_TXE));
		usart_disable_tx_interrupt(MODBUS_UART);
		while(!usart_get_flag( MODBUS_UART, USART_SR_TC)){}


		usart_enable_rx_interrupt(MODBUS_UART);
		gpio_clear(MODBUS_DE_PORT, MODBUS_DE_PIN);
	}

	if(xTxEnable && !xRxEnable)
	{
		usart_disable_rx_interrupt(MODBUS_UART);

		gpio_set(MODBUS_DE_PORT, MODBUS_DE_PIN);
		//for (int i = 0; i<5000; i++);

		usart_enable_tx_interrupt(MODBUS_UART);
	}

	if (!xRxEnable && !xTxEnable) {
		while( !usart_get_flag( MODBUS_UART, USART_SR_TXE));
		usart_disable_tx_interrupt(MODBUS_UART);
		while( !usart_get_flag( MODBUS_UART, USART_SR_TC));

		usart_disable_rx_interrupt(MODBUS_UART);
	}
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	BOOL bStatus = TRUE;

	/* Clock and GPIO Uart */
	rcc_periph_clock_enable(RCC_USART1);
	gpio_set_mode(MODBUS_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, MODBUS_TX_PIN);
	gpio_set_mode(MODBUS_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, MODBUS_RX_PIN); //GPIO_CNF_INPUT_PULL_UPDOWN



	/* DE config */
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, MODBUS_DE_PIN);
	gpio_clear(GPIOB, GPIO0);

	/* END */


	/* Setup UART parameters. */
	usart_set_baudrate(MODBUS_UART, MODBUS_BAUDRATE);
	usart_set_databits(MODBUS_UART, MODBUS_DATABITS);
	usart_set_stopbits(MODBUS_UART, MODBUS_STOPBITS);
	usart_set_mode(MODBUS_UART, MODBUS_MODE);
	usart_set_flow_control(MODBUS_UART, MODBUS_FLOWCONTROL);

	switch ( eParity )
	{
	case MB_PAR_NONE:
		usart_set_parity(MODBUS_UART, USART_PARITY_NONE);
		break;
	case MB_PAR_ODD:
		usart_set_parity(MODBUS_UART, USART_PARITY_ODD);
		break;
	case MB_PAR_EVEN:
		usart_set_parity(MODBUS_UART, USART_PARITY_EVEN);
		break;
	default:
		bStatus = FALSE;
		break;
	}

	/* Enable the USART1 interrupt. */
	nvic_set_priority(MODBUS_IRQ, 1<<7);
	nvic_enable_irq(MODBUS_IRQ);

	if( bStatus == TRUE )
	{
		/* Finally enable the USART. */
		//usart_disable_rx_interrupt(MODBUS_UART);
		usart_enable_rx_interrupt(MODBUS_UART);
		usart_disable_tx_interrupt(MODBUS_UART);
		usart_enable(MODBUS_UART);
	}
	return bStatus;

}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
	usart_send(MODBUS_UART, ((uint8_t)ucByte));

	return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
	*pucByte = (CHAR) (usart_recv(MODBUS_UART));

	return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
/*static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}*/

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
/*static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}*/


void MODBUS_IRQ_HANDLER(void)
 {
	/* Check if we were called because of RXNE. */
	if (((USART_CR1(MODBUS_UART) & USART_CR1_RXNEIE) != 0) && usart_get_flag(MODBUS_UART, USART_SR_RXNE))
	{
		pxMBFrameCBByteReceived();
	}
	/* Check if we were called because of TXE. */
	if (((USART_CR1(MODBUS_UART) & USART_CR1_TXEIE) != 0) && usart_get_flag(MODBUS_UART, USART_SR_TXE))
	{
		pxMBFrameCBTransmitterEmpty();
	}
}
