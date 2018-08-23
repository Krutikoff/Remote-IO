#pragma once
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

    using Queue = etl::queue<Message, 7>;

    void run();
    Queue* get_queue() { return &_queue;}
private:
    Queue _queue;
};

