#pragma once
#include <Modbus/registers.h>
#include <cstdint>
#include <etl/queue.h>
#include <libopencm3/stm32/exti.h>
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

    // using Queue = etl::queue<Message, 7>;

    static GpioModule& instance()
    {
        static GpioModule instance;
        return instance;
    }

    void run();

 private:
    // Queue _queue;
    etl::array<GpioIO, 8> _gpio = {
      {{GpioMode::READ, {GPIOA, GPIO0}, {GPIOC, GPIO0}},
       {GpioMode::READ, {GPIOA, GPIO1}, {GPIOC, GPIO1}},
       {GpioMode::READ, {GPIOA, GPIO2}, {GPIOC, GPIO2}},
       {GpioMode::READ, {GPIOA, GPIO3}, {GPIOC, GPIO3}},
       {GpioMode::READ, {GPIOB, GPIO6}, {GPIOC, GPIO4}},
       {GpioMode::READ, {GPIOB, GPIO7}, {GPIOC, GPIO5}},
       {GpioMode::READ, {GPIOB, GPIO8}, {GPIOC, GPIO6}},
       {GpioMode::READ, {GPIOB, GPIO9}, {GPIOC, GPIO7}}}};
    etl::array<uint8_t, 8> _exti = {EXTI0, EXTI1, EXTI2, EXTI3,
                                    EXTI4, EXTI5, EXTI6, EXTI7};
    etl::array<uint8_t, 8> _cached_gpio_write = {0};

    GpioModule();
    GpioModule(const GpioModule&) = default;
    GpioModule& operator=(const GpioModule&);

    void _dispatch_queue();
    void _polling_gpio_registers();

    void _latch_clear();
    void _counter_clear();

    friend void exti0_isr(void);
    friend void exti1_isr(void);
    friend void exti2_isr(void);
    friend void exti3_isr(void);
    friend void exti4_isr(void);
    friend void exti9_5_isr(void);
};
