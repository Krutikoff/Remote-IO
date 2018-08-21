#pragma once

#include <etl/queue.h>
#include "gpio_module.h"

class PollingRegistersModule
{
    using GpioQueue = GpioModule::Queue;
public:

    PollingRegistersModule(GpioQueue *gpio_queue);
    void run();

private:
    GpioQueue* _gpio_queue;
};

