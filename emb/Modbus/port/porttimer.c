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
 * File: $Id: porttimer.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "../../Modbus/port/port.h"
#include "mb.h"
#include "mbport.h"

#include "libopencm3/stm32/timer.h"
#include "libopencm3/stm32/f1/nvic.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

static uint16_t timeout = 0;
static uint16_t downcounter = 0;

/* ----------------------- static functions ---------------------------------*/
//static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	/* Enable TIM2 clock. */
		//rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);
#if 1
	rcc_periph_clock_enable(RCC_TIM2);
	nvic_enable_irq(NVIC_TIM2_IRQ);
	nvic_set_priority(NVIC_TIM2_IRQ, 1);

	/* Timer global mode: - Divider 1, Alignment edge, Direction up */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_continuous_mode(TIM2);
	timer_set_prescaler(TIM2, (rcc_apb1_frequency * 2) / 100000); /* 72MHz to 50 microseconds period */
	timer_set_period(TIM2, 5);
	timeout = usTim1Timerout50us;
	timer_enable_counter(TIM2);
#endif

#if 0
	rcc_periph_clock_enable(RCC_TIM2);

	nvic_enable_irq(NVIC_TIM2_IRQ);
	nvic_set_priority(NVIC_TIM2_IRQ, 1);



	TIM_CNT(TIM2) = 1;

	/* Set timer prescaler. 72MHz/1440 => 50000 counts per second. */
	TIM_PSC(TIM2) = (rcc_apb1_frequency * 2) / 100000;

	TIM_ARR(TIM2) = 5; //2.5

	TIM_DIER(TIM2) |= TIM_DIER_UIE;

	TIM_CR1(TIM2) |= TIM_CR1_CEN;
#endif






	return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
	downcounter = timeout;
	timer_enable_irq(TIM2, TIM_DIER_UIE);
}

inline void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
	timer_disable_irq(TIM2, TIM_DIER_UIE);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
/*static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}*/

void tim2_isr(void)
{
	//gpio_toggle(GPIOA, GPIO0);   /* LED2 on/off. */

	//TIM_SR(TIM2) &= ~TIM_SR_UIF; /* Clear interrrupt flag. */


	if (timer_interrupt_source(TIM2, TIM_SR_UIF))
	{
		timer_clear_flag(TIM2, TIM_SR_UIF); /* Clear interrrupt flag. */
		if (!--downcounter)
			pxMBPortCBTimerExpired();
	}
}



