#pragma once

#include <inttypes.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>


class Module {

public:

	void rcc_clock_config ();
	void mb_gpio_config();
	void mb_exti_config();
	void remap();
	void led_setup();

	void Conf_PWM_TIM(uint32_t tim);
	void tim3_config();

	void usart_setup();

private:


};

