/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
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
 * File: $Id: porttimer.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
//#include "tpl_timer.h"
//#include "tpl_convert_stm32_std.h"
#include "libopencmsis/core_cm3.h"
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/timer.h"
#include "libopencm3/stm32/gpio.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

static uint16_t timeout = 0;
static uint16_t downcounter = 0;

/* ----------------------- static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)
{
	/* Enable TIM2 clock. */
	//rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);
	rcc_periph_clock_enable(RCC_TIM2);


 	nvic_enable_irq(NVIC_TIM2_IRQ);
	timer_reset(TIM2);
 	//timer_clear_flag(TIM_SR_UIF); // Clear Update interrupt flag
	/* Timer global mode: - Divider 1, Alignment edge, Direction up */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_continuous_mode(TIM2);
	timer_set_prescaler(TIM2, 29); /* 72MHz to 50 microseconds period */
	timer_set_period(TIM2, 49);
	timeout = usTim1Timerout50us;

    return TRUE;
}

void vMBPortTimersEnable()
{
    /* Restart the timer with the period value set in xMBPortTimersInit( ) */
	downcounter = timeout;
	timer_enable_irq(TIM2, TIM_DIER_UIE);
}

void vMBPortTimersDisable()
{
	timer_disable_irq(TIM2, TIM_DIER_UIE);
}

void tim2_isr(void)
{
	if (timer_interrupt_source(TIM2, TIM_SR_UIF))
	{
		timer_clear_flag(TIM2, TIM_SR_UIF); /* Clear interrrupt flag. */
		if (!--downcounter)
			pxMBPortCBTimerExpired();
	}
}
