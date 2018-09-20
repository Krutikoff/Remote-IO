#pragma once

#include <libopencm3/stm32/gpio.h>

class ErrorModule
{
 public:
    static ErrorModule& instance()
    {
        static ErrorModule instance;
        return instance;
    }

    void error_indicator_on();
    void error_indicator_off();

 private:
    static constexpr uint16_t _LED_ERROR = GPIO4;

    ErrorModule();
    ErrorModule(const ErrorModule&) = default;
    ErrorModule& operator=(const ErrorModule&);
};
