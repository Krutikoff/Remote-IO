#include "flash_module.h"
#include <Modbus/modbus.h>

uint32_t FlashModule::run()
{
    _polling_flash_data();

    if (_flag_write) { _write_data_to_flash(); }
    return 0;
}

void FlashModule::_polling_flash_data()
{

    Modbus& modbus = Modbus::instance();

    UsartModule& usart_module = UsartModule::instance();
    FlashModule& flash_module = FlashModule::instance();

    auto gpio_setup_registers =
      modbus.get_iterator<GpioModule::GpioMode>(Coil::GPIO_SET_BIT0);

    GpioModule::GpioMode current_gpio_mode;

    for (uint32_t i = 0U; i < 8; ++i) {

        current_gpio_mode = gpio_setup_registers[i];

        if (flash_module._flash_data.gpio_mode[i] != current_gpio_mode) {
            flash_module._flash_data.gpio_mode[i] =
              (current_gpio_mode == GpioModule::GpioMode::WRITE)
                ? GpioModule::GpioMode::WRITE
                : GpioModule::GpioMode::READ;
            _flag_write = true;
            _flash_data.flash_gpio_flag = _FLASH_GPIO_FLAG;
        }
    }

    if (flash_module._flash_data.baudrate != *usart_module.get_baudrate()) {
        flash_module._flash_data.baudrate = *usart_module.get_baudrate();
        _flag_write = true;
        _flash_data.flash_uart_baudrate_flag = _FLASH_UART_BAUDRATE_FLAG;
    }
}

uint32_t FlashModule::_write_data_to_flash()
{
    uint32_t flash_status = 0;

    FlashData* _data_ptr = &_flash_data;
    uint32_t* start_addr_data = (uint32_t*)_data_ptr;
    uint32_t start_memory_flash = _OPERATION_ADDRESS;

    flash_unlock();
    flash_erase_page(_OPERATION_ADDRESS);
    flash_status = flash_get_status_flags();
    if (flash_status != FLASH_SR_EOP) return flash_status;

    for (uint32_t i = 0U; i < _size_flash_data(_flash_data); ++i) {

        flash_program_word(start_memory_flash, *(start_addr_data + i));
        start_memory_flash = start_memory_flash + _step_unit_address;
        flash_status = flash_get_status_flags();
        if (flash_status != FLASH_SR_EOP) return flash_status;
    }

    // Clear flag in buffer _flash_data.
    _flash_data.flash_gpio_flag = 0;
    _flash_data.flash_uart_baudrate_flag = 0;
    _flag_write = false;
    return 0;
}

uint32_t FlashModule::read_data_from_flash()
{
    Modbus& modbus = Modbus::instance();

    auto gpio_setup_registers =
      modbus.get_iterator<GpioModule::GpioMode>(Coil::GPIO_SET_BIT0);
    auto baudrate_registers =
      modbus.get_iterator<uint32_t>(Holding::UART_BAUDRATE_0);

    // Status flash_gpio_flag and flash_uart_baudrate_flag save on flash
    // If Gpio was changed then gpio module read data(gpio_mode) from flash
    // If Baudrate was changed then uart module read data(baudrate) from flash

    for (uint32_t i = 0U; i < _flash_data_save->gpio_mode.size(); ++i) {
        if (_flash_data_save->flash_gpio_flag ==
            _FLASH_GPIO_FLAG) {  // !!!WARNING!!!
            gpio_setup_registers[i] = _flash_data_save->gpio_mode[i];
            _flash_data.gpio_mode[i] = _flash_data_save->gpio_mode[i];
        }
    }

    if (_flash_data_save->flash_uart_baudrate_flag ==
        _FLASH_UART_BAUDRATE_FLAG) {
        *baudrate_registers = _flash_data_save->baudrate;
        _flash_data.baudrate = _flash_data_save->baudrate;
    }

    return 0;
}

uint32_t FlashModule::_size_flash_data(FlashData flash_data)
{

    uint16_t size = sizeof(flash_data);

    if (size % 4 != 0) { size = (size / 4) + 1; }
    else
        size = (size / 4);

    return size;
}
