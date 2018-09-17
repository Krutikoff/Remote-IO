#pragma once
#include <Modbus/registers.h>
#include <cstdint>
#include <etl/queue.h>

class UsartModule
{
 public:
    enum Action
    {

    };

    struct Message
    {
        Action action;
        uint32_t address;
        uint32_t value;
    };

    static UsartModule& instance()
    {
        static UsartModule instance;
        return instance;
    }

    void run();
    uint32_t* get_baudrate() { return &_baudrate; };

 private:
    uint32_t _baudrate = 0;
    static constexpr uint8_t _ERROR_BAUDRATE = 0x01;
    static constexpr uint8_t _ERROR_BAUDRATE_OK = 0x00;
    static constexpr etl::array<uint32_t, 7> _baudrate_standart_value = {
      1200, 4800, 9600, 19200, 38400, 57600, 115200};
    uint32_t baudrate_array[7] = {1200, 4800, 9600, 19200, 38400, 57600, 115200};

    void _baudrate_set();
};
