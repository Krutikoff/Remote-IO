#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/common/rcc_common_all.h>
#include <libopencm3/stm32/timer.h>
#include <Modbus/modbus.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>

#include <iterator>
#include <algorithm>


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


void rcc_clock_config ( void ) {
	rcc_clock_setup_in_hse_16mhz_out_72mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(rcc_periph_clken::RCC_AFIO);
}

void led_setup( void ) {
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO0 | GPIO1 | GPIO2 | GPIO3);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO6 | GPIO7 | GPIO8 | GPIO9 );
}

void nvic_setup( void) {
	nvic_enable_irq(NVIC_TIM2_IRQ);
	nvic_set_priority(NVIC_TIM2_IRQ, 1);
}

void tim2_config( void ) {

	rcc_periph_clock_enable(RCC_TIM2);

	TIM_CNT(TIM2) = 1;

	/* Set timer prescaler. 72MHz/1440 => 50000 counts per second. */
	TIM_PSC(TIM2) = 1440;

	TIM_ARR(TIM2) = 3; //2.5

	TIM_DIER(TIM2) |= TIM_DIER_UIE;

	TIM_CR1(TIM2) |= TIM_CR1_CEN;
}

static void usart_setup(void) {
	/* Enable clocks for GPIO port A (for GPIO_USART1_TX) and USART1. */
	rcc_periph_clock_enable(RCC_USART1);

	/* Setup GPIO pin GPIO_USART1_TX/GPIO9 on GPIO port A for transmit. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
	              GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART1);
}


int main()
{
	/* Set GPIO3 and GPIO4 (in GPIO port C) to 'output push-pull'. */
	//gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6 | GPIO8);

	rcc_clock_config();
	led_setup();
	//nvic_setup();
	gpio_clear(GPIOA, GPIO0);
	//tim2_config();
	//usart_setup();

	//gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO0);
	//gpio_set(GPIOB, GPIO0);


	// LED PIN: PA1, PA2, PA3, PB6, PB7, PB8, PB9

	//gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0 | GPIO1 | GPIO2 | GPIO3);
	//gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO6 | GPIO7 | GPIO8 | GPIO9);
	//gpio_set(GPIOA, GPIO0 | GPIO1 | GPIO2 | GPIO3);
	//gpio_set(GPIOB, GPIO6 | GPIO7 | GPIO8 | GPIO9);

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


	//rcc_periph_clock_enable(RCC_TIM4);
	//Timer Timer4;
	//Timer4.Config_PWM(TIM4);


	Modbus& modbus = Modbus::instance();



	uint8_t arr_coil[7] = {1, 0, 1, 0, 1, 0, 1};
	//uint8_t arr_coil_copy[4] = {0};
	uint8_t* it = modbus.get_iterator <uint8_t>(0x0001);
	std::copy_n(arr_coil, 7, it);
	while(true) {

		modbus.poll();

		//br[0] = 0x01;

		//char* const end = it + 5;
		//const char* it_br = br;



		RegisterType type_coil = modbus._get_type(0x0001);
		RegisterType type_discrete = modbus._get_type(0x1001);
		RegisterType type_input = modbus._get_type(0x3001);
		RegisterType type_holding = modbus._get_type(0x4001);

		if(*it){
			gpio_set(GPIOA, GPIO0);
		}
		else
			gpio_clear(GPIOA, GPIO0);

		if(*(it+1)){
			gpio_set(GPIOA, GPIO1);
		}
		else
			gpio_clear(GPIOA, GPIO1);

		if(*(it+2)){
			gpio_set(GPIOA, GPIO2);
		}
		else
			gpio_clear(GPIOA, GPIO2);

		if(*(it+3)){
			gpio_set(GPIOA, GPIO3);
		}
		else
			gpio_clear(GPIOA, GPIO3);
		if(*(it+4)){
			gpio_set(GPIOB, GPIO6);
		}
		else
			gpio_clear(GPIOB, GPIO6);
		if(*(it+5)){
			gpio_set(GPIOB, GPIO7);
		}
		else
			gpio_clear(GPIOB, GPIO7);
		if(*(it+6)){
			gpio_set(GPIOB, GPIO8);
		}
		else
			gpio_clear(GPIOB, GPIO8);
		if(*(it+7)){
			gpio_set(GPIOB, GPIO9);
		}
		else
			gpio_clear(GPIOB, GPIO9);


		/*while(it != end){

			*it++ = it_br++;
		}*/



//
//		for(int i = 0; i < 4; i++){
//
//
//		modbus.get_register(COIL, i, value_coil[i]);
//
//		if( value_coil[i] ) {
//				gpio_set(GPIOA, 1 << i);
//		}
//		else
//			gpio_clear(GPIOA, ~(1 << i));
//	}


	//usart_send(USART1, 'H');
//
//		for(int PWM_pulse = 0 ; PWM_pulse < 65535; PWM_pulse += 100) {
//
//			/*timer_set_oc_value(TIM2, TIM_OC1 , PWM_pulse);
//			timer_set_oc_value(TIM2, TIM_OC2 , PWM_pulse);
//			timer_set_oc_value(TIM2, TIM_OC3 , PWM_pulse);
//			timer_set_oc_value(TIM2, TIM_OC4 , PWM_pulse);*/
//
//			timer_set_oc_value(TIM4, TIM_OC1 , PWM_pulse);
//			timer_set_oc_value(TIM4, TIM_OC2 , PWM_pulse);
//			timer_set_oc_value(TIM4, TIM_OC3 , PWM_pulse);
//			timer_set_oc_value(TIM4, TIM_OC4 , PWM_pulse);
//			for (int i = 0; i < 800; i++);
//		}
//		for(int PWM_pulse = 65535 ; PWM_pulse > 0; PWM_pulse -=  100) {
//			/*timer_set_oc_value(TIM2, TIM_OC1 , PWM_pulse);
//			timer_set_oc_value(TIM2, TIM_OC2 , PWM_pulse);
//			timer_set_oc_value(TIM2, TIM_OC3 , PWM_pulse);
//			timer_set_oc_value(TIM2, TIM_OC4 , PWM_pulse);*/
//
//			timer_set_oc_value(TIM4, TIM_OC1 , PWM_pulse);
//			timer_set_oc_value(TIM4, TIM_OC2 , PWM_pulse);
//			timer_set_oc_value(TIM4, TIM_OC3 , PWM_pulse);
//			timer_set_oc_value(TIM4, TIM_OC4 , PWM_pulse);
//			for (int i = 0; i < 800; i++);
//		}

			//__asm__("nop");
	}

	return 0;
}
