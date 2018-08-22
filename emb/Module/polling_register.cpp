#include "Module/polling_registers.h"

PollingRegistersModule::PollingRegistersModule(GpioQueue *gpio_queue,
                                               UsartQueue* usart_queue,
                                               I2cQueue* i2c_queue) :
    _gpio_queue(gpio_queue), _usart_queue(usart_queue), _i2c_queue(i2c_queue)
{

}

void PollingRegistersModule::run()
{
//    GpioModule::Message gpio_msg;
//    UsartModule::Message usart_msg;
//    I2cModule::Message i2c_msg;

    _gpio_queue->push(_gpio_msg);
    _usart_queue->push(_usart_msg);
    _i2c_queue->push(i2c_msg);
}

void PollingRegistersModule::_polling_gpio_registers()
{
    Modbus& modbus = Modbus::instance();
    auto gpio_setup_registers = modbus.get_iterator<GpioModule::GpioMode>(Coil::GPIO_SET_BIT0);

    for (uint32_t i = 0U; i < cached_gpio_mode.size(); ++i) {
        auto current_pin_mode = gpio_setup_registers[i];


        if (cached_gpio_mode[i] != current_pin_mode) {
            _gpio_msg.address = static_cast<GpioModule::Address>(i);
            _gpio_msg.action = (current_pin_mode == GpioModule::GpioMode::WRITE) ?
                GpioModule::Action::SETUP_WRITE :
                GpioModule::Action::SETUP_READ;

            _gpio_queue->push(_gpio_msg);
        }

        cached_gpio_mode[i] = current_pin_mode;
    }


}

