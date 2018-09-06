#pragma once

#include "Module/gpio_module.h"
#include "Module/usart_module.h"
#include <cstdint>
#include <etl/queue.h>
#include <libopencm3/stm32/flash.h>

class FlashModule
{
 public:
    using FlashGpioIO = GpioModule::GpioIO;
    using FlashLatchStatus = GpioModule::LatchStatus;

    struct Latch
    {
        etl::array<FlashLatchStatus, 8> low = {
          FlashLatchStatus::HIGH, FlashLatchStatus::HIGH,
          FlashLatchStatus::HIGH, FlashLatchStatus::HIGH,
          FlashLatchStatus::HIGH, FlashLatchStatus::HIGH,
          FlashLatchStatus::HIGH, FlashLatchStatus::HIGH};
        etl::array<FlashLatchStatus, 8> high = {
          FlashLatchStatus::LOW, FlashLatchStatus::LOW, FlashLatchStatus::LOW,
          FlashLatchStatus::LOW, FlashLatchStatus::LOW, FlashLatchStatus::LOW,
          FlashLatchStatus::LOW, FlashLatchStatus::LOW};
    };

    struct __attribute__((packed, aligned(1))) FlashData
    {
        etl::array<GpioModule::GpioMode, 8> gpio_mode;  // 8 bytes
        uint32_t baudrate = 0;                          // 4 bytes
    };

    static_assert((sizeof(FlashData) == 12), "FlashData must be 12 bytes");

    static FlashModule& instance()
    {
        static FlashModule instance;
        return instance;
    }

    uint32_t run();
    uint32_t read_data_from_flash();
    FlashData* get_save_data() { return _flash_data_save; }

 private:
    static constexpr uint32_t _OPERATION_ADDRESS = 0x08007C00;  // last page

    uint32_t* _memory_ptr = (uint32_t*)_OPERATION_ADDRESS;
    FlashData* _flash_data_save = (FlashData*)_memory_ptr;
    FlashData _flash_data;
    bool _flag_write = false;

    void _polling_flash_data();
    uint32_t _write_data_to_flash();
};
