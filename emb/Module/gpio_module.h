#pragma once
#include <cstdint>
#include <etl/queue.h>
#include <Modbus/registers.h>


class GpioModule
{
public:



    enum class Action
    {
        SETUP_WRITE, SETUP_READ, SET_PIN, LATCH_CLEAR,
    };

    enum class Address
    {
        PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7,
    };

    enum class GpioMode: uint8_t {
        READ, WRITE,
    };

    enum class LatchStatus: uint8_t {
        LOW, HIGH, RESET, SET
    };



    struct Message
    {
        Action action;
        Address address;
        uint32_t value;
    };

    using Queue = etl::queue<Message, 7>;

    void run();
    Queue* get_queue() { return &_queue;}

private:
    Queue _queue;
};
