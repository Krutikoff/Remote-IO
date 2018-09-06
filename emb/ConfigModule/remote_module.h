#pragma once

#include <inttypes.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>

#include <Modbus/modbus.h>
#include <Module/flash_module.h>
#include <Module/gpio_module.h>
#include <Module/i2c_module.h>
#include <Module/usart_module.h>

class RemoteModule
{

 public:
    void config();
    void run();

    static RemoteModule& instance()
    {
        static RemoteModule instance;
        return instance;
    }

    //RemoteModule();

 private:
    GpioModule& _gpio_module;
    UsartModule& _usart_module;
    I2cModule _i2c_module;
    FlashModule& _flash_module;

    RemoteModule();
    RemoteModule(const RemoteModule&) = default;
    RemoteModule& operator=(const RemoteModule&);

    void _rcc_clock_config();
    void _gpio_config();
    void _exti_config();
    void _remap();
    void _led_setup();
    void _Conf_PWM_TIM(uint32_t tim);
    void _tim3_config();
    void _usart_setup();

    friend void tim3_isr(void);
};
