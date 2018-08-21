#pragma once

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
};

