#include "Module/gpio_module.h"

void GpioModule::run()
{
    //_dispatch_queue();
    //  if (!_queue.empty()) _queue.clear();
    _polling_gpio_registers();
}

void GpioModule::_dispatch_queue()
{

    etl::array<uint16_t, 8> gpio_pin = {GPIO0, GPIO1, GPIO2, GPIO3,
                                        GPIO6, GPIO7, GPIO8, GPIO9};

    GpioModule::Message current_gpio_module_msg;

    if (current_gpio_module_msg.action == GpioModule::Action::SET_PIN) {

        uint8_t address = static_cast<uint8_t>(current_gpio_module_msg.address);
        // uint8_t address =(uint8_t)current_gpio_module_msg.address;
        if (current_gpio_module_msg.value == 1 && address <= 3)
            gpio_set(GPIOA, gpio_pin[address]);
        else if (current_gpio_module_msg.value == 1 && address > 3)
            gpio_set(GPIOB, gpio_pin[address]);
        else if (current_gpio_module_msg.value == 0 && address <= 3)
            gpio_clear(GPIOA, gpio_pin[address]);
        else if (current_gpio_module_msg.value == 0 && address > 3)
            gpio_clear(GPIOB, gpio_pin[address]);
    }
    else if (current_gpio_module_msg.action == GpioModule::Action::SETUP_READ) {
    }
    else if (current_gpio_module_msg.action ==
             GpioModule::Action::SETUP_WRITE) {
    }
}

void GpioModule::_polling_gpio_registers()
{

    Modbus& modbus = Modbus::instance();
    auto gpio_setup_registers =
      modbus.get_iterator<GpioModule::GpioMode>(Coil::GPIO_SET_BIT0);

    for (uint32_t i = 0U; i < _gpio.size(); ++i) {
        auto current_pin_mode = gpio_setup_registers[i];

        if (_gpio[i].mode != current_pin_mode) {
            _gpio[i].mode = (current_pin_mode == GpioModule::GpioMode::WRITE)
                              ? GpioModule::GpioMode::WRITE
                              : GpioModule::GpioMode::READ;

            if (_gpio[i].mode == GpioModule::GpioMode::WRITE) {
                exti_disable_request(_exti[i]);
            }
            else if (_gpio[i].mode == GpioModule::GpioMode::READ) {
                exti_enable_request(_exti[i]);
            }
        }
    }

    auto gpio_write_registers =
      modbus.get_iterator<uint8_t>(Coil::GPIO_WRITE_BIT0);

    for (uint32_t i = 0U; i < _gpio.size(); ++i) {
        auto current_gpio_write = gpio_write_registers[i];

        if (_cached_gpio_write[i] != current_gpio_write) {
            if (current_gpio_write == 1 &&
                _gpio[i].mode == GpioModule::GpioMode::WRITE) {
                gpio_set(_gpio[i].port_write.gpio_port,
                         _gpio[i].port_write.gpio_pin);
            }
            else if (current_gpio_write == 0 &&
                     _gpio[i].mode == GpioModule::GpioMode::WRITE) {
                gpio_clear(_gpio[i].port_write.gpio_port,
                           _gpio[i].port_write.gpio_pin);
            }
        }
        _cached_gpio_write[i] = current_gpio_write;
    }
}
