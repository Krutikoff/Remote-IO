#include "usart_module.h"
#include <Modbus/modbus.h>

void UsartModule::run()
{
    _baudrate_set();
}

void UsartModule::_baudrate_set()
{

    Modbus& modbus = Modbus::instance();
    ErrorModule& error_module = ErrorModule::instance();

    auto baudrate_registers =
      modbus.get_iterator<uint32_t>(Holding::UART_BAUDRATE_0);
    auto error_uart_baudrate =
         modbus.get_iterator<uint16_t>(Input::ERROR_BAUDRATE_UART);

    bool is_baudrate_correct = false;

    for (/*uint32_t baud : _baudrate_standart_value*/ uint32_t baud = 0;
         baud < 7; ++baud) {
        if (*baudrate_registers == baudrate_array[baud]) {
            is_baudrate_correct = true;
            error_module.error_indicator_off();  // Off LED ERROR
            if(*error_uart_baudrate != _ERROR_BAUDRATE_OK)
                *error_uart_baudrate = _ERROR_BAUDRATE_OK;
            break;
        }
    }

    if (_baudrate != *baudrate_registers &&
        is_baudrate_correct) {  // Change baudrate Uart

        nvic_disable_irq(MODBUS_IRQ);
        usart_disable(MODBUS_UART);
        _baudrate = *baudrate_registers;
        usart_set_baudrate(MODBUS_UART, _baudrate);
        nvic_enable_irq(MODBUS_IRQ);
        usart_enable(MODBUS_UART);
    }
    else if (!is_baudrate_correct) {
        error_module.error_indicator_on();  // ON LED ERROR
        if(*error_uart_baudrate != _ERROR_BAUDRATE)
            *error_uart_baudrate = _ERROR_BAUDRATE;
        return;
    }
}
