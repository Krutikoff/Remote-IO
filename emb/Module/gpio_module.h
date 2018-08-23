#pragma once
#include <Modbus/registers.h>
#include <cstdint>
#include <etl/queue.h>
#include <libopencm3/stm32/gpio.h>

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

    using Queue = etl::queue<Message, 7>;

    void run();
    Queue* get_queue() { return &_queue; }

 private:
    Queue _queue;
    Message _cache_gpio_module_msg;

    void _dispatch_queue();
};
