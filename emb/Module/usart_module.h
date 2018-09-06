#pragma once
#include <cstdint>
#include <Modbus/registers.h>

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

    static UsartModule& instance(){
        static UsartModule instance;
        return instance;
    }

    void run();
    uint32_t* get_baudrate(){return &_baudrate;};
private:
    uint32_t _baudrate = 0;

    void _baudrate_set();
};

