#include "flash_module.h"

uint32_t FlashModule::run()
{

//    static uint32_t write_data = 12345;
//    static uint32_t read_data = 0;
//    uint32_t flash_status = 0;

//    size_t size_flash = sizeof(_flash_data);
//    size_t size_gpio_mode = sizeof(FlashData::gpio_mode);
//    size_t size_gpio_write = sizeof(FlashData::gpio_write);
//    size_t size_read = sizeof(FlashData::gpio_read);
//    size_t size_latch = sizeof(Latch);
    //static bool flash_switch = true;

    _polling_flash_data();

    if(flag_write){
        _write_data_to_flash();

    }
//    if (read_data != write_data) {
//        flash_unlock();
//        flash_erase_page(OPERATION_ADDRESS);
//        flash_program_word(OPERATION_ADDRESS, write_data);
//        flash_status = flash_get_status_flags();
//        if (flash_status != FLASH_SR_EOP) return flash_status;
//        read_data = *_memory_ptr;
//    }

    return 0;
}

void FlashModule::_polling_flash_data()
{

    Modbus& modbus = Modbus::instance();

    UsartModule& usart_module = UsartModule::instance();
    FlashModule& flash_module = FlashModule::instance();

    auto gpio_setup_registers =
      modbus.get_iterator<GpioModule::GpioMode>(Coil::GPIO_SET_BIT0);
    auto gpio_write_registers =
      modbus.get_iterator<uint8_t>(Coil::GPIO_WRITE_BIT0);
    auto latch_low_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_LOW);
    auto latch_high_registers =
      modbus.get_iterator<GpioModule::LatchStatus>(Discrete::LATCH0_HIGH);
    auto counter_registers =
      modbus.get_iterator<uint16_t>(Input::COUNTER0_VALUE);

    GpioModule::GpioMode current_gpio_mode;
    uint8_t current_gpio_write;

    for (uint32_t i = 0U; i < 8; ++i) {

        current_gpio_mode = gpio_setup_registers[i];
        current_gpio_write = gpio_write_registers[i];

        if (flash_module._flash_data.gpio_mode[i] != current_gpio_mode) {
            flash_module._flash_data.gpio_mode[i] =
              (current_gpio_mode == GpioModule::GpioMode::WRITE)
                ? GpioModule::GpioMode::WRITE
                : GpioModule::GpioMode::READ;
            flag_write = true;
        }

        if (flash_module._flash_data.gpio_write[i] != current_gpio_write) {
            flash_module._flash_data.gpio_write[i] = current_gpio_write;
            flag_write = true;
        }

        if (flash_module._flash_data.latch.low[i] != latch_low_registers[i]) {
            flash_module._flash_data.latch.low[i] = latch_low_registers[i];
            flag_write = true;
        }

        if (flash_module._flash_data.latch.high[i] != latch_high_registers[i]) {
            flash_module._flash_data.latch.high[i] = latch_high_registers[i];
            flag_write = true;
        }

        if (flash_module._flash_data.counter[i] != counter_registers[i]) {
            flash_module._flash_data.counter[i] = counter_registers[i];
            flag_write = true;
        }
    }

    if (flash_module._flash_data.baudrate != *usart_module.get_baudrate()) {
        flash_module._flash_data.baudrate = *usart_module.get_baudrate();
        flag_write = true;
    }
}

uint32_t FlashModule::_write_data_to_flash()
{

    FlashModule& flash_module = FlashModule::instance();

    uint32_t flash_status = 0;

    FlashData* _data_ptr = &_flash_data;
    uint32_t* start_addr_data = (uint32_t*)_data_ptr;
    uint32_t start_addr_flash = _OPERATION_ADDRESS;

    flash_unlock();
    flash_erase_page(_OPERATION_ADDRESS);
    flash_status = flash_get_status_flags();
    if(flash_status != FLASH_SR_EOP)
        return flash_status;

    uint32_t num_elements = ( sizeof(_flash_data) ) / 4; // 60 bytes / 4 = 15

    FlashData* new_flash_data;

    new_flash_data = (FlashData*)_memory_ptr;
    for (uint32_t i = 0U; i < num_elements; ++i) {

        flash_program_word(start_addr_flash, *(start_addr_data + i));
        start_addr_flash = start_addr_flash + 4;
        flash_status = flash_get_status_flags();
        if (flash_status != FLASH_SR_EOP) return flash_status;

    }
    new_flash_data = (FlashData*)_memory_ptr;

    flag_write = false;
    return 0;
}
