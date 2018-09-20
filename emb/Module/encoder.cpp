#include "encoder.h"

EncoderModule::EncoderModule()
{
    _address_device = (uint8_t) ((~((gpio_port_read(GPIOA) >> 4))) & 0x000F);
}
