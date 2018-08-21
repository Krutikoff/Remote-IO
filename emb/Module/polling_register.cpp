#include "Module/polling_registers.h"

PollingRegistersModule::PollingRegistersModule(GpioQueue *gpio_queue) :
    _gpio_queue(gpio_queue)
{

}

void PollingRegistersModule::run()
{
    GpioModule::Message msg;

    _gpio_queue->push(msg);
}

