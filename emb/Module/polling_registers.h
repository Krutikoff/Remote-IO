#pragma once

#include <Modbus/modbus.h>

#include <etl/queue.h>
#include "gpio_module.h"
#include "usart_module.h"
#include "i2c_module.h"

class PollingRegistersModule
{
    using GpioQueue = GpioModule::Queue;
    using UsartQueue = UsartModule::Queue;
    using I2cQueue = I2cModule::Queue;
public:

    PollingRegistersModule(GpioQueue *gpio_queue, UsartQueue* usart_queue,
                           I2cQueue* i2c_queue);
    void run();

private:

    GpioQueue* _gpio_queue;
    UsartQueue* _usart_queue;
    I2cQueue* _i2c_queue;

    GpioModule::Message _gpio_msg;
    UsartModule::Message _usart_msg;
    I2cModule::Message _i2c_msg;

    etl::array<GpioModule::GpioMode, 8> _cached_gpio_mode = { GpioModule::GpioMode::READ };
    etl::array<uint8_t, 8> _cached_gpio_write = { 0 };
    etl::array<uint8_t, 8> _cached_gpio_read = { 0 };
    etl::array<GpioModule::LatchStatus, 8> _cached_latch_status = { GpioModule::LatchStatus::LOW };

    void _polling_gpio_registers();
    void _polling_latch_registers();

    void _polling_discrete_registers();
    void _polling_input_registers();
    void _polling_holding_registers();
};

