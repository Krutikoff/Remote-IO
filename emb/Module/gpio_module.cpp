#include "Module/gpio_module.h"
#include <Modbus/modbus.h>

GpioModule* gpio_ptr;

GpioModule::GpioModule()
{
    gpio_ptr = this;
}

void GpioModule::run()
{
    _polling_gpio_registers();
    _latch_clear();
    _counter_clear();
}

void GpioModule::_polling_gpio_registers()
{

    Modbus& modbus = Modbus::instance();
    auto gpio_setup_registers =
      modbus.get_iterator<GpioModule::GpioMode>(Coil::GPIO_SET_BIT0);

    for (uint32_t i = 0U; i < _gpio.size(); ++i) {
        auto current_pin_mode = gpio_setup_registers[i];

        // If change, then write new value
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

        if(_gpio[i].mode == GpioModule::GpioMode::WRITE) {
            // If change, then write new value
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
}

void exti0_isr(void)
{
    auto address = static_cast<uint8_t>(GpioModule::Address::PIN0);

    Modbus& modbus = Modbus::instance();
    GpioModule& gpio_module = GpioModule::instance();

    exti_reset_request(gpio_module._exti[address]);  // Reset flag

    static bool front_rising = false;
    static bool front_falling = false;

    auto gpio_read_registers =
      modbus.get_iterator<uint8_t>(Discrete::GPIO_READ_BIT0);
    auto latch_low_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_LOW);
    auto latch_high_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_HIGH);
    auto counter_registers =
      modbus.get_iterator<uint16_t>(Input::COUNTER0_VALUE);

    // READ_GPIO
    gpio_read_registers[address] =
      (uint8_t)(gpio_get(gpio_module._gpio[address].port_read.gpio_port,
                         gpio_module._gpio[address].port_read.gpio_pin));
    // Latch_low
    if (gpio_read_registers[address]) {  // LATCH_LOW
        front_rising = true;
        latch_low_registers[address] = GpioModule::LatchStatus::LOW;
        latch_high_registers[address] = GpioModule::LatchStatus::LOW;
    }
    else {  // LATCH_HIGH
        front_falling = true;
        latch_high_registers[address] = GpioModule::LatchStatus::HIGH;
        latch_low_registers[address] = GpioModule::LatchStatus::HIGH;
    }

    if (front_rising && front_falling) {

        counter_registers[address]++;
        if (counter_registers[address] > gpio_module.MAX_VALUE_COUNTER)
            counter_registers[address] = 0;
        front_rising = false;
        front_falling = false;
    }
}

void exti1_isr(void)
{
    auto address = static_cast<uint8_t>(GpioModule::Address::PIN1);

    GpioModule& gpio_module = GpioModule::instance();
    Modbus& modbus = Modbus::instance();

    exti_reset_request(gpio_module._exti[address]);  // Reset flag

    static bool front_rising = false;
    static bool front_falling = false;

    auto gpio_read_registers =
      modbus.get_iterator<uint8_t>(Discrete::GPIO_READ_BIT0);
    auto latch_low_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_LOW);
    auto latch_high_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_HIGH);
    auto counter_registers =
      modbus.get_iterator<uint16_t>(Input::COUNTER0_VALUE);

    // READ_GPIO
    gpio_read_registers[address] =
      (uint8_t)((gpio_get(gpio_module._gpio[address].port_read.gpio_port,
                          gpio_module._gpio[address].port_read.gpio_pin)) >>
                1);
    // Latch_low
    if (gpio_read_registers[address]) {  // LATCH_LOW
        front_rising = true;
        latch_low_registers[address] = GpioModule::LatchStatus::LOW;
        latch_high_registers[address] = GpioModule::LatchStatus::LOW;
    }
    else {  // LATCH_HIGH
        front_falling = true;
        latch_high_registers[address] = GpioModule::LatchStatus::HIGH;
        latch_low_registers[address] = GpioModule::LatchStatus::HIGH;
    }

    if (front_rising && front_falling) {

        counter_registers[address]++;
        if (counter_registers[address] > gpio_module.MAX_VALUE_COUNTER)
            counter_registers[address] = 0;
        front_rising = false;
        front_falling = false;
    }
}

void exti2_isr(void)
{
    auto address = static_cast<uint8_t>(GpioModule::Address::PIN2);

    GpioModule& gpio_module = GpioModule::instance();
    Modbus& modbus = Modbus::instance();

    exti_reset_request(gpio_module._exti[address]);  // Reset flag

    static bool front_rising = false;
    static bool front_falling = false;

    auto gpio_read_registers =
      modbus.get_iterator<uint8_t>(Discrete::GPIO_READ_BIT0);
    auto latch_low_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_LOW);
    auto latch_high_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_HIGH);
    auto counter_registers =
      modbus.get_iterator<uint16_t>(Input::COUNTER0_VALUE);

    // READ_GPIO
    gpio_read_registers[address] =
      (uint8_t)((gpio_get(gpio_module._gpio[address].port_read.gpio_port,
                          gpio_module._gpio[address].port_read.gpio_pin)) >>
                2);
    // Latch_low
    if (gpio_read_registers[address]) {  // LATCH_LOW
        front_rising = true;
        latch_low_registers[address] = GpioModule::LatchStatus::LOW;
        latch_high_registers[address] = GpioModule::LatchStatus::LOW;
    }
    else {  // LATCH_HIGH
        front_falling = true;
        latch_high_registers[address] = GpioModule::LatchStatus::HIGH;
        latch_low_registers[address] = GpioModule::LatchStatus::HIGH;
    }

    if (front_rising && front_falling) {

        counter_registers[address]++;
        if (counter_registers[address] > gpio_module.MAX_VALUE_COUNTER)
            counter_registers[address] = 0;
        front_rising = false;
        front_falling = false;
    }
}

void exti3_isr(void)
{
    auto address = static_cast<uint8_t>(GpioModule::Address::PIN3);

    GpioModule& gpio_module = GpioModule::instance();
    Modbus& modbus = Modbus::instance();

    exti_reset_request(gpio_module._exti[address]);  // Reset flag

    static bool front_rising = false;
    static bool front_falling = false;

    auto gpio_read_registers =
      modbus.get_iterator<uint8_t>(Discrete::GPIO_READ_BIT0);
    auto latch_low_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_LOW);
    auto latch_high_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_HIGH);
    auto counter_registers =
      modbus.get_iterator<uint16_t>(Input::COUNTER0_VALUE);

    // READ_GPIO
    gpio_read_registers[address] =
      (uint8_t)((gpio_get(gpio_module._gpio[address].port_read.gpio_port,
                          gpio_module._gpio[address].port_read.gpio_pin)) >>
                3);
    // Latch_low
    if (gpio_read_registers[address]) {  // LATCH_LOW
        front_rising = true;
        latch_low_registers[address] = GpioModule::LatchStatus::LOW;
        latch_high_registers[address] = GpioModule::LatchStatus::LOW;
    }
    else {  // LATCH_HIGH
        front_falling = true;
        latch_high_registers[address] = GpioModule::LatchStatus::HIGH;
        latch_low_registers[address] = GpioModule::LatchStatus::HIGH;
    }

    if (front_rising && front_falling) {
        counter_registers[address]++;
        if (counter_registers[address] > gpio_module.MAX_VALUE_COUNTER)
            counter_registers[address] = 0;
        front_rising = false;
        front_falling = false;
    }
}

void exti4_isr(void)
{
    auto address = static_cast<uint8_t>(GpioModule::Address::PIN4);

    GpioModule& gpio_module = GpioModule::instance();
    Modbus& modbus = Modbus::instance();

    exti_reset_request(gpio_module._exti[address]);  // Reset flag

    static bool front_rising = false;
    static bool front_falling = false;

    auto gpio_read_registers =
      modbus.get_iterator<uint8_t>(Discrete::GPIO_READ_BIT0);
    auto latch_low_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_LOW);
    auto latch_high_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_HIGH);
    auto counter_registers =
      modbus.get_iterator<uint16_t>(Input::COUNTER0_VALUE);

    // READ_GPIO
    gpio_read_registers[address] =
      (uint8_t)((gpio_get(gpio_module._gpio[address].port_read.gpio_port,
                          gpio_module._gpio[address].port_read.gpio_pin)) >>
                4);
    // Latch_low
    if (gpio_read_registers[address]) {  // LATCH_LOW
        front_rising = true;
        latch_low_registers[address] = GpioModule::LatchStatus::LOW;
        latch_high_registers[address] = GpioModule::LatchStatus::LOW;
    }
    else {  // LATCH_HIGH
        front_falling = true;
        latch_high_registers[address] = GpioModule::LatchStatus::HIGH;
        latch_low_registers[address] = GpioModule::LatchStatus::HIGH;
    }

    if (front_rising && front_falling) {

        counter_registers[address]++;
        if (counter_registers[address] > gpio_module.MAX_VALUE_COUNTER)
            counter_registers[address] = 0;
        front_rising = false;
        front_falling = false;
    }
}
void exti9_5_isr(void)
{
    GpioModule& gpio_module = GpioModule::instance();
    Modbus& modbus = Modbus::instance();

    auto gpio_read_registers =
      modbus.get_iterator<uint8_t>(Discrete::GPIO_READ_BIT0);
    auto latch_low_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_LOW);
    auto latch_high_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_HIGH);
    auto counter_registers =
      modbus.get_iterator<uint16_t>(Input::COUNTER0_VALUE);

    static bool front_rising[8] = {false};
    static bool front_falling[8] = {false};

    for (uint8_t i = static_cast<uint8_t>(GpioModule::Address::PIN5);
         i <= static_cast<uint8_t>(GpioModule::Address::PIN7); ++i) {

        if (exti_get_flag_status(gpio_module._exti[i])) {
            exti_reset_request(gpio_module._exti[i]);  // Reset flag

            // READ_GPIO
            gpio_read_registers[i] =
              (uint8_t)(gpio_get(gpio_module._gpio[i].port_read.gpio_port,
                                 gpio_module._gpio[i].port_read.gpio_pin) >>
                        i);
            // Latch_low
            if (gpio_read_registers[i]) {  // LATCH_LOW
                front_rising[i] = true;
                latch_low_registers[i] = GpioModule::LatchStatus::LOW;
                latch_high_registers[i] = GpioModule::LatchStatus::LOW;
            }
            else {  // LATCH_HIGH
                front_falling[i] = true;
                latch_high_registers[i] = GpioModule::LatchStatus::HIGH;
                latch_low_registers[i] = GpioModule::LatchStatus::HIGH;
            }

            if (front_rising[i] && front_falling[i]) {

                counter_registers[i]++;
                if (counter_registers[i] > gpio_module.MAX_VALUE_COUNTER)
                    counter_registers[i] = 0;
                front_rising[i] = false;
                front_falling[i] = false;
            }
        }
    }
}

void GpioModule::_latch_clear()
{

    Modbus& modbus = Modbus::instance();

    auto latch_low_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_LOW);
    auto latch_high_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_HIGH);
    uint8_t* latch_clear = modbus.get_iterator<uint8_t>(Coil::LATCH_CLEAR);

    GpioModule::LatchStatus clear_low[8] = {GpioModule::LatchStatus::HIGH,
                                            GpioModule::LatchStatus::HIGH,
                                            GpioModule::LatchStatus::HIGH,
                                            GpioModule::LatchStatus::HIGH,
                                            GpioModule::LatchStatus::HIGH,
                                            GpioModule::LatchStatus::HIGH,
                                            GpioModule::LatchStatus::HIGH,
                                            GpioModule::LatchStatus::HIGH};
    GpioModule::LatchStatus clear_high[8] = {GpioModule::LatchStatus::LOW};

    if (*latch_clear) {
        etl::copy_n(clear_low, 8, latch_low_registers);
        etl::copy_n(clear_high, 8, latch_high_registers);
        *latch_clear = 0U;
    }
}

void GpioModule::_counter_clear()
{

    Modbus& modbus = Modbus::instance();

    uint8_t* counter_clear_registers =
      modbus.get_iterator<uint8_t>(Coil::COUNTER0_CLEAR);

    uint16_t* counter_registers =
      modbus.get_iterator<uint16_t>(Input::COUNTER0_VALUE);

    for (uint32_t i = 0U; i < _gpio.size(); ++i) {
        if (counter_clear_registers[i]) {
            counter_registers[i] = 0;
            counter_clear_registers[i] = 0;
        }
    }
}
