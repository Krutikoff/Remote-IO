#include <Modbus/modbus.h>

#include <algorithm>
#include <iterator>
#include <queue>

#include <ConfigModule/config_module.h>
#include <Module/gpio_module.h>
#include <Module/i2c_module.h>
#include <Module/usart_module.h>

#include <Module/flash_module.h>

int main()
{
    Module module;
    GpioModule& gpio_module = GpioModule::instance();
    UsartModule& usart_module = UsartModule::instance();
    I2cModule i2c_module;
    FlashModule& flash_module = FlashModule::instance();

    module.rcc_clock_config();
    // module.remap(); REMAP FUNCRION
    module.led_setup();
    // nvic_setup();
    // gpio_clear(GPIOA, GPIO0);
    // tim2_config();
    // usart_setup();

    module.mb_gpio_config();

    Modbus& modbus = Modbus::instance();

    module.mb_exti_config();

    while (true) {

        modbus.poll();
        gpio_module.run();
        usart_module.run();
        i2c_module.run();
        flash_module.run();

        //        modbus.mb_baudrate_set();

        //__asm__("nop");
    }

    return 0;
}
