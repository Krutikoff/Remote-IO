#include "Module/polling_registers.h"

PollingRegistersModule::PollingRegistersModule(GpioQueue *gpio_queue,
                                               UsartQueue* usart_queue,
                                               I2cQueue* i2c_queue) :
    _gpio_queue(gpio_queue), _usart_queue(usart_queue), _i2c_queue(i2c_queue)
{

}

void PollingRegistersModule::run()
{
    GpioModule::Message gpio_msg;
    UsartModule::Message usart_msg;
    I2cModule::Message i2c_msg;

    _gpio_queue->push(gpio_msg);
    _usart_queue->push(usart_msg);
    _i2c_queue->push(i2c_msg);
}

