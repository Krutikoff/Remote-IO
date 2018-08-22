#include <Modbus/modbus.h>

#include <iterator>
#include <algorithm>
#include <queue>

#include <ConfigModule/config_module.h>
#include <Module/polling_registers.h>
#include <Module/gpio_module.h>
#include <Module/usart_module.h>
#include <Module/i2c_module.h>

uint8_t* Modbus::coils_it_gpio_set;
uint8_t* Modbus::coils_it_gpio_write;
uint8_t* Modbus::coils_it_latch_clear;

int main()
{
    Module module;
    GpioModule gpio_module;
    UsartModule usart_module;
    I2cModule i2c_module;
    PollingRegistersModule polling_registers(gpio_module.get_queue(),
                                             usart_module.get_queue(),
                                             i2c_module.get_queue());

    module.rcc_clock_config();
    //module.remap(); REMAP FUNCRION
    module.led_setup();
    //nvic_setup();
    //gpio_clear(GPIOA, GPIO0);
    //tim2_config();
    //usart_setup();

    module.mb_gpio_config();

    //etl::queue<int, 3> queue;

    Modbus& modbus = Modbus::instance();

    module.mb_exti_config();

    modbus.coils_it_gpio_set = modbus.get_iterator<uint8_t>(0x0001);
    modbus.coils_it_gpio_write = modbus.get_iterator<uint8_t>(0x0009);
    modbus.coils_it_latch_clear = modbus.get_iterator<uint8_t>(0x0011);

    //std::queue<message_queue_gpio> queue_gpio;

    while (true) {

        modbus.poll();

        polling_registers.run();
        gpio_module.run();
        usart_module.run();
        i2c_module.run();

        modbus.mb_gpio_set(modbus.coils_it_gpio_set);

        modbus.mb_gpio_write(modbus.coils_it_gpio_write);

        if (*modbus.coils_it_latch_clear) {
            modbus.mb_latch_clear();
            *modbus.coils_it_latch_clear = 0;
        }

        modbus.mb_counter_clear();
        modbus.mb_baudrate_set();

        //__asm__("nop");
    }

    return 0;
}
