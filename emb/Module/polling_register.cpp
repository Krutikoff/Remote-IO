#include "Module/polling_registers.h"

PollingRegistersModule::PollingRegistersModule(GpioQueue* gpio_queue,
                                               UsartQueue* usart_queue,
                                               I2cQueue* i2c_queue) :
  _gpio_queue(gpio_queue),
  _usart_queue(usart_queue), _i2c_queue(i2c_queue)
{
}

void PollingRegistersModule::run()
{
    _polling_gpio_registers();
    _polling_latch_registers();
}

void PollingRegistersModule::_polling_gpio_registers()
{
    Modbus& modbus = Modbus::instance();
    auto gpio_setup_registers =
      modbus.get_iterator<GpioModule::GpioMode>(Coil::GPIO_SET_BIT0);

    for (uint32_t i = 0U; i < _cached_gpio_mode.size(); ++i) {
        auto current_pin_mode = gpio_setup_registers[i];

        if (_cached_gpio_mode[i] != current_pin_mode) {
            _gpio_msg.address = static_cast<GpioModule::Address>(i);
            _gpio_msg.action = (current_pin_mode == GpioModule::GpioMode::WRITE)
                                 ? GpioModule::Action::SETUP_WRITE
                                 : GpioModule::Action::SETUP_READ;
            _gpio_queue->push(_gpio_msg);
        }

        _cached_gpio_mode[i] = current_pin_mode;
    }

    // Write 0 or 1 to GPIO_WRITE registers Modbus
    auto gpio_write_registers =
      modbus.get_iterator<uint8_t>(Coil::GPIO_WRITE_BIT0);

    for (uint32_t i = 0U; i < _cached_gpio_write.size(); ++i) {
        auto current_gpio_write = gpio_write_registers[i];

        if (_cached_gpio_write[i] != current_gpio_write) {
            _gpio_msg.action = GpioModule::Action::SET_PIN;
            _gpio_msg.address = static_cast<GpioModule::Address>(i);
            _gpio_msg.value = (current_gpio_write);
            _gpio_queue->push(_gpio_msg);
        }
        _cached_gpio_write[i] = current_gpio_write;
    }

    // Read from GPIO_READ registers Modbus
}

void PollingRegistersModule::_polling_latch_registers()
{
    Modbus& modbus = Modbus::instance();
    // Latch Clear
    auto latch_status =
      modbus.get_iterator<GpioModule::LatchStatus>(Coil::LATCH_CLEAR);
    if (*latch_status == GpioModule::LatchStatus::SET) {
        _gpio_msg.action = GpioModule::Action::LATCH_CLEAR;
        _gpio_msg.value = static_cast<uint32_t>(*latch_status);
        *latch_status = GpioModule::LatchStatus::RESET;
    }
}
