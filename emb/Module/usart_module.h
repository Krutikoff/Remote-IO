#pragma once
#include <cstdint>
#include <Modbus/registers.h>
//#include <etl/queue.h>

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

    //using Queue = etl::queue<Message, 7>;

    void run();
    uint32_t* get_baudrate(){return &_baudrate;};
    //Queue* get_queue() { return &_queue;}
private:
   // Queue _queue;
    uint32_t _baudrate = 0;

    void _baudrate_set();
};

