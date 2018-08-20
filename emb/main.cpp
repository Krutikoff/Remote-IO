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

	Modbus& modbus = Modbus::instance();

	module.mb_exti_config();

	modbus.coils_it_gpio_set = modbus.get_iterator <uint8_t>(0x0001);
	modbus.coils_it_gpio_write = modbus.get_iterator <uint8_t>(0x0009);
	modbus.coils_it_latch_clear = modbus.get_iterator <uint8_t>(0x0011);

	//std::queue<message_queue_gpio> queue_gpio;

	while(true) {

		modbus.poll();

		modbus.mb_gpio_set(modbus.coils_it_gpio_set);

		modbus.mb_gpio_write(modbus.coils_it_gpio_write);

		if(*modbus.coils_it_latch_clear){
			modbus.mb_latch_clear();
			*modbus.coils_it_latch_clear = 0;
		}

		modbus.mb_counter_clear();
		modbus.mb_baudrate_set();

			//__asm__("nop");
	}

	return 0;
}
