//#include <libopencm3/stm32/rcc.h>
//#include <libopencm3/stm32/gpio.h>
//#include <libopencm3/stm32/common/rcc_common_all.h>
//#include <libopencm3/stm32/timer.h>
#include <Modbus/modbus.h>

#include <iterator>
#include <algorithm>
#include <queue>

#include <ConfigModule/config_module.h>

uint8_t* Modbus::coils_it_gpio_set;
uint8_t* Modbus::coils_it_gpio_write;
uint8_t* Modbus::coils_it_latch_clear;


 enum gpio{
	WRITE,
	READ,
	SET,
};

struct message_queue_gpio{
	gpio action;
	uint32_t address;
	uint32_t value;

};


int main()
{
	Module module;

	module.rcc_clock_config();
	module.led_setup();
	//nvic_setup();
	//gpio_clear(GPIOA, GPIO0);
	//tim2_config();
	//usart_setup();


	module.mb_gpio_config();
	//EXTI interrupt


	Modbus& modbus = Modbus::instance();

	module.mb_exti_config();

	//uint8_t arr_coil[7] = {1, 0, 1, 0, 1, 0, 1};
	//uint8_t arr_coil_copy[4] = {0};
	//uint8_t* coil_it_set = modbus.get_iterator <uint8_t>(0x0001);
	//uint8_t* coil_it_write = modbus.get_iterator <uint8_t>(0x0009);

	modbus.coils_it_gpio_set = modbus.get_iterator <uint8_t>(0x0001);
	modbus.coils_it_gpio_write = modbus.get_iterator <uint8_t>(0x0009);
	modbus.coils_it_latch_clear = modbus.get_iterator <uint8_t>(0x0011);

	//uint8_t* discrete_data = modbus.get_iterator <uint8_t>(0x1001);

	//uint8_t* discrete_it_latch_low = modbus.get_iterator <uint8_t>(0x1009);

	//uint8_t* input_it = modbus.get_iterator <uint8_t>(0x3001);

	//uint8_t* holding_it = modbus.get_iterator <uint8_t>(0x4001);

	//std::copy_n(arr_coil, 7, coil_it_write);


//	message_queue_gpio message_queue;
//	uint8_t data_coil = 0;
//	data_coil = *coil_it;
//	if(data_coil != *coil_it){
//		data_coil = *coil_it;
//		message_queue.action = gpio::WRITE;
//		message_queue.address = 0x0001;
//		if(*data_coil){
//			message_queue.value = 1;
//		}
//		else{
//			message_queue.value = 0;
//		}
//	}

	std::queue<message_queue_gpio> queue_gpio;



	while(true) {

		modbus.poll();


		modbus.mb_gpio_set(modbus.coils_it_gpio_set);


//		if(modbus.coils_it_gpio_set == WRITE_MODE){
//			modbus.write_mode_run();
//		}

		modbus.mb_gpio_write(modbus.coils_it_gpio_write);

		if(*modbus.coils_it_latch_clear){
			modbus.mb_latch_clear();
			*modbus.coils_it_latch_clear = 0;
		}

		modbus.mb_counter_clear();
		modbus.mb_baudrate_set();
		//mb_gpio_set(coil_it_set);


			//__asm__("nop");
	}

	return 0;
}
