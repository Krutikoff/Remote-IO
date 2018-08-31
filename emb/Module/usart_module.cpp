#include "usart_module.h"
#include <Modbus/modbus.h>

void UsartModule::run()
{
    _baudrate_set();
    //if (!_queue.empty()) _queue.clear();
}


void UsartModule::_baudrate_set() {

    Modbus& modbus = Modbus::instance();

    auto baudrate_registers =
       modbus.get_iterator<uint32_t>(Holding::UART_BAUDRATE_0);

    if (_baudrate != *baudrate_registers) {

        nvic_disable_irq(MODBUS_IRQ);
        usart_disable(MODBUS_UART);
        _baudrate = *baudrate_registers;
        usart_set_baudrate(MODBUS_UART, _baudrate);
        nvic_enable_irq(MODBUS_IRQ);
        usart_enable(MODBUS_UART);
    }
}
