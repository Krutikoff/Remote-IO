#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/common/rcc_common_all.h>
#include <libopencm3/stm32/timer.h>


void Conf_PWM_TIM(uint32_t tim) {
	timer_set_mode(tim, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1,
			TIM_CR1_DIR_UP);
	timer_set_oc_mode(tim, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_mode(tim, TIM_OC2 , TIM_OCM_PWM1);
	timer_set_oc_mode(tim, TIM_OC3 , TIM_OCM_PWM1);
	timer_set_oc_mode(tim, TIM_OC4 , TIM_OCM_PWM1);
	timer_enable_oc_output(tim, TIM_OC1);
	timer_enable_oc_output(tim, TIM_OC2);
	timer_enable_oc_output(tim, TIM_OC3);
	timer_enable_oc_output(tim, TIM_OC4);
	timer_enable_break_main_output(tim);
	timer_enable_counter(tim);

	/*rcc_periph_clock_enable(RCC_TIM2);
	TIM2_CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E|TIM_CCER_CC4E; // ENABLE
	TIM2_CCMR1 |= TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC2M_PWM1;
	TIM2_CCMR2 |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC4M_PWM1;
	TIM2_CR1 = TIM_CR1_CEN;*/
	/*rcc_periph_clock_enable(RCC_TIM4);
	TIM4_CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E|TIM_CCER_CC4E; // ENABLE
	TIM4_CCMR1 |= TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC2M_PWM1;
	TIM4_CCMR2 |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC4M_PWM1;
	TIM4_CR1 = TIM_CR1_CEN;*/
}


class Timer {
public:
	/*Timer( uint32_t tim ) {
		Config(tim);
	}*/
	void Config_PWM( uint32_t tim){
		timer_set_mode(tim, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1,
		TIM_CR1_DIR_UP);
		timer_set_oc_mode(tim, TIM_OC1, TIM_OCM_PWM1);
		timer_set_oc_mode(tim, TIM_OC2 , TIM_OCM_PWM1);
		timer_set_oc_mode(tim, TIM_OC3 , TIM_OCM_PWM1);
		timer_set_oc_mode(tim, TIM_OC4 , TIM_OCM_PWM1);
		timer_enable_oc_output(tim, TIM_OC1);
		timer_enable_oc_output(tim, TIM_OC2);
		timer_enable_oc_output(tim, TIM_OC3);
		timer_enable_oc_output(tim, TIM_OC4);
		timer_enable_break_main_output(tim);
		timer_enable_counter(tim);
	}

};


int main()
{
	/* Set GPIO3 and GPIO4 (in GPIO port C) to 'output push-pull'. */
	//gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6 | GPIO8);



	// LED PIN: PA1, PA2, PA3, PB6, PB7, PB8, PB9

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(rcc_periph_clken::RCC_AFIO);
	//RCC_APB2ENR |= RCC_APB2ENR_AFIOEN; // alternate function push-pull outputs where the PWM


	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0 | GPIO1 | GPIO2 | GPIO3);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO6 | GPIO7 | GPIO8 | GPIO9);
	gpio_set(GPIOA, GPIO0 | GPIO1 | GPIO2 | GPIO3);
	gpio_set(GPIOB, GPIO6 | GPIO7 | GPIO8 | GPIO9);

	/*****************TIMER********************/
	/* PA0 - TIM2_CH1
	 * PA1 - TIM2_CH2
	 * PA2 - TIM2_CH1
	 * PA3 - TIM2_CH1
	 * PB6 - TIM4_CH1
	 * PB7 - TIM4_CH1
	 * PB8 - TIM4_CH1
	 * PB9 - TIM4_CH1
	 * ****************************************/

	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_clock_enable(RCC_TIM4);
	//Conf_PWM_TIM(TIM2);
	//Conf_PWM_TIM(TIM4);
	Timer Timer2, Timer4;
	Timer2.Config_PWM(TIM2);
	Timer4.Config_PWM(TIM4);

	while(true) {

		//gpio_toggle(GPIOA, GPIO0 | GPIO1 | GPIO2 | GPIO3);



		for(int PWM_pulse = 0 ; PWM_pulse < 65535; PWM_pulse += 100) {

			timer_set_oc_value(TIM2, TIM_OC1 , PWM_pulse);
			timer_set_oc_value(TIM2, TIM_OC2 , PWM_pulse);
			timer_set_oc_value(TIM2, TIM_OC3 , PWM_pulse);
			timer_set_oc_value(TIM2, TIM_OC4 , PWM_pulse);

			timer_set_oc_value(TIM4, TIM_OC1 , PWM_pulse);
			timer_set_oc_value(TIM4, TIM_OC2 , PWM_pulse);
			timer_set_oc_value(TIM4, TIM_OC3 , PWM_pulse);
			timer_set_oc_value(TIM4, TIM_OC4 , PWM_pulse);
			for (int i = 0; i < 800; i++);
		}
		for(int PWM_pulse = 65535 ; PWM_pulse > 0; PWM_pulse -=  100) {
			timer_set_oc_value(TIM2, TIM_OC1 , PWM_pulse);
			timer_set_oc_value(TIM2, TIM_OC2 , PWM_pulse);
			timer_set_oc_value(TIM2, TIM_OC3 , PWM_pulse);
			timer_set_oc_value(TIM2, TIM_OC4 , PWM_pulse);

			timer_set_oc_value(TIM4, TIM_OC1 , PWM_pulse);
			timer_set_oc_value(TIM4, TIM_OC2 , PWM_pulse);
			timer_set_oc_value(TIM4, TIM_OC3 , PWM_pulse);
			timer_set_oc_value(TIM4, TIM_OC4 , PWM_pulse);
			for (int i = 0; i < 800; i++);
		}
			//__asm__("nop");
	}
	return 0;
}
