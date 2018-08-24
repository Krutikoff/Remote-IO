#pragma once
#include <Modbus/registers.h>
#include <cstdint>
#include <etl/queue.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>



class GpioModule
{
 public:
    enum class Action
    {
        SETUP_WRITE,
        SETUP_READ,
        SET_PIN,
        LATCH_CLEAR,
    };

    enum class Address : uint8_t
    {
        PIN0,
        PIN1,
        PIN2,
        PIN3,
        PIN4,
        PIN5,
        PIN6,
        PIN7,
    };

    enum class GpioMode : uint8_t
    {
        READ,
        WRITE,
    };

    enum class LatchStatus : uint8_t
    {
        LOW,
        HIGH,
        RESET,
        SET
    };

    struct Message
    {
        Action action;
        Address address;
        uint32_t value;
    };

    struct PortPin
    {
        uint32_t gpio_port;
        uint16_t gpio_pin;
    };

    struct GpioIO
    {
        GpioMode mode;
        PortPin port_write;
        PortPin port_read;
    };

    using Queue = etl::queue<Message, 7>;


    GpioModule();


    void run();
    Queue* get_queue() { return &_queue; }
    //auto get_gpio() { return _gpio; } // GpioIO* get_gpio() { return _gpio; } ERROR?


    etl::array<GpioIO, 8> _gpio = {{
      {GpioMode::READ, {GPIOA, GPIO0}, {GPIOC, GPIO0} },
      {GpioMode::READ, {GPIOA, GPIO1}, {GPIOC, GPIO1} },
      {GpioMode::READ, {GPIOA, GPIO2}, {GPIOC, GPIO2} },
      {GpioMode::READ, {GPIOA, GPIO3}, {GPIOC, GPIO3} },
      {GpioMode::READ, {GPIOB, GPIO6}, {GPIOC, GPIO4} },
      {GpioMode::READ, {GPIOB, GPIO7}, {GPIOC, GPIO5} },
      {GpioMode::READ, {GPIOB, GPIO8}, {GPIOC, GPIO6} },
      {GpioMode::READ, {GPIOB, GPIO9}, {GPIOC, GPIO7} }
    }};

    etl::array<uint8_t, 8> _exti = { EXTI0, EXTI1, EXTI2, EXTI3, EXTI4, EXTI5, EXTI6, EXTI7 };

 private:
    Queue _queue;
    etl::array<uint8_t, 8> _cached_gpio_write = { 0 };



    void _dispatch_queue();
    void _polling_gpio_registers();

};
