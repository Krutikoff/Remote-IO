#include "error_module.h"

ErrorModule::ErrorModule() {}

void ErrorModule::error_indicator_off()
{
    gpio_set(GPIOB, _LED_ERROR);  // Off LED ERROR}
}

void ErrorModule::error_indicator_on()
{
    gpio_clear(GPIOB, _LED_ERROR);  // On LED ERROR}
}
