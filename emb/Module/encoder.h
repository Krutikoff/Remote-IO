#pragma once

#include <cstdint>
#include <libopencm3/stm32/gpio.h>

class EncoderModule
{
 public:
    static EncoderModule& instance()
    {

        static EncoderModule instanse;
        return instanse;
    }

    uint8_t get_address_device() { return _address_device; }

 private:
    uint8_t _address_device;

    EncoderModule();
    EncoderModule(const EncoderModule&) = default;
    EncoderModule& operator=(const EncoderModule);
};
