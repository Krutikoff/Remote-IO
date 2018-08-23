#include "usart_module.h"

void UsartModule::run()
{
    if (!_queue.empty()) _queue.clear();
}
