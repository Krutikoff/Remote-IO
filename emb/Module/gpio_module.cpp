#include "Module/gpio_module.h"

void GpioModule::run()
{
    _dispatch_queue();
    //  if (!_queue.empty()) _queue.clear();
}

void GpioModule::_dispatch_queue()
{

    etl::array<uint16_t, 8> gpio_pin = {GPIO0, GPIO1, GPIO2, GPIO3,
                                        GPIO6, GPIO7, GPIO8, GPIO9};

    GpioModule::Message current_gpio_module_msg;

    while (!_queue.empty()) {

        current_gpio_module_msg = _queue.front();
        _queue.pop();

        if (current_gpio_module_msg.action == GpioModule::Action::SET_PIN) {

            uint8_t address =
              static_cast<uint8_t>(current_gpio_module_msg.address);
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
        else if (current_gpio_module_msg.action ==
                 GpioModule::Action::SETUP_READ) {
        }
        else if (current_gpio_module_msg.action ==
                 GpioModule::Action::SETUP_WRITE) {
        }
    }
}
