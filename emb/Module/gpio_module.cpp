#include "Module/gpio_module.h"

void GpioModule::run()
{
    if (!_queue.empty()) _queue.clear();
}

