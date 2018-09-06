#pragma once
#include <cstdint>


class I2cModule
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



    void run();

private:

};
