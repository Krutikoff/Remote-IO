#include <ConfigModule/remote_module.h>

RemoteModule::RemoteModule() :
  _gpio_module(GpioModule::instance()), _usart_module(UsartModule::instance()),
  _flash_module(FlashModule::instance())
{
    config();

    etl::array<GpioModule::GpioIO, 8> gpio_data = _gpio_module.get_qpio();
    FlashModule::FlashData* flash_data = _flash_module.get_save_data();

    for (uint32_t i = 0U; i < gpio_data.size(); ++i) {
        if (flash_data->gpio_mode[i] != GpioModule::GpioMode::READ) {
            _flash_module.read_data_from_flash();
        }
    }

    if (flash_data->baudrate != 0xFFFFFFFF) {
        _flash_module.read_data_from_flash();
    }
}

void RemoteModule::config()
{
    _rcc_clock_config();
    _led_setup();
    _gpio_config();
    _exti_config();
}

void RemoteModule::run()
{
    _gpio_module.run();
    _usart_module.run();
    _i2c_module.run();
    _flash_module.run();
}

void RemoteModule::_rcc_clock_config()
{
    rcc_clock_setup_in_hse_16mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(rcc_periph_clken::RCC_AFIO);
}

void RemoteModule::_remap()
{
    // gpio_primary_remap( AFIO_MAPR_SWJ_CFG_FULL_SWJ,AFIO_MAPR_SWJ_MASK);
    AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON;
}

void RemoteModule::_gpio_config()
{
    // Config pins for GPIOA and GPIOB to output mode
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO0 | GPIO1 | GPIO2 | GPIO3);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO6 | GPIO7 | GPIO8 | GPIO9);
    // Config GPIO for read address. With pull up to power through register ODR
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN,
                  GPIO4 | GPIO5 | GPIO6 | GPIO7);
    gpio_set(GPIOA, GPIO4 | GPIO5 | GPIO6 | GPIO7);
    // Config GPIOC to input mode
    gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                  GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO4 | GPIO5 | GPIO6 |
                    GPIO7);
}

void RemoteModule::_exti_config()
{
    nvic_enable_irq(NVIC_EXTI0_IRQ);
    nvic_enable_irq(NVIC_EXTI1_IRQ);
    nvic_enable_irq(NVIC_EXTI2_IRQ);
    nvic_enable_irq(NVIC_EXTI3_IRQ);
    nvic_enable_irq(NVIC_EXTI4_IRQ);
    nvic_enable_irq(NVIC_EXTI9_5_IRQ);

    exti_select_source(
      EXTI0 | EXTI1 | EXTI2 | EXTI3 | EXTI4 | EXTI5 | EXTI6 | EXTI7, GPIOC);
    exti_set_trigger(EXTI0 | EXTI1 | EXTI2 | EXTI3 | EXTI4 | EXTI5 | EXTI6 |
                       EXTI7,
                     EXTI_TRIGGER_BOTH);
    exti_enable_request(EXTI0 | EXTI1 | EXTI2 | EXTI3 | EXTI4 | EXTI5 | EXTI6 |
                        EXTI7);
}

void RemoteModule::_led_setup()
{
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO0 | GPIO1 | GPIO2 | GPIO3);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO6 | GPIO7 | GPIO8 | GPIO9);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  GPIO3 | GPIO4);
    gpio_set(GPIOB, GPIO3 | GPIO4);
}

void RemoteModule::_tim3_config()
{
    rcc_periph_clock_enable(RCC_TIM3);
    nvic_enable_irq(NVIC_TIM3_IRQ);
    nvic_set_priority(NVIC_TIM3_IRQ, 2);

    /* Timer global mode: - Divider 1, Alignment edge, Direction up */
    timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_continuous_mode(TIM3);
    timer_set_prescaler(TIM3, (rcc_apb1_frequency * 2) /
                                100000); /* 72MHz to 50 microseconds period */
    timer_set_period(TIM3, 5);
    timer_enable_counter(TIM3);

#if 0
	rcc_periph_clock_enable(RCC_TIM2);

	TIM_CNT(TIM2) = 1;

	/* Set timer prescaler. 72MHz/1440 => 50000 counts per second. */
	TIM_PSC(TIM2) = 1440;

	TIM_ARR(TIM2) = 3; //2.5

	TIM_DIER(TIM2) |= TIM_DIER_UIE;

	TIM_CR1(TIM2) |= TIM_CR1_CEN;
#endif
}

void tim3_isr(void)
{
    TIM_SR(TIM3) &= ~TIM_SR_UIF; /* Clear interrrupt flag. */
}

void RemoteModule::_Conf_PWM_TIM(uint32_t tim)
{
    // LED PIN: PA1, PA2, PA3, PB6, PB7, PB8, PB9

    // gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
    // GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0 | GPIO1 | GPIO2 | GPIO3);
    // gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
    // GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO6 | GPIO7 | GPIO8 | GPIO9);
    // gpio_set(GPIOA, GPIO0 | GPIO1 | GPIO2 | GPIO3);
    // gpio_set(GPIOB, GPIO6 | GPIO7 | GPIO8 | GPIO9);

    /*****************TIMER********************/
    /* PA0 - TIM2_CH1
     * PA1 - TIM2_CH2
     * PA2 - TIM2_CH1
     * PA3 - TIM2_CH1
     * PB6 - TIM4_CH1
     * PB7 - TIM4_CH1
     * PB8 - TIM4_CH1
     * PB9 - TIM4_CH1
     * ****************************************/

    timer_set_mode(tim, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1,
                   TIM_CR1_DIR_UP);
    timer_set_oc_mode(tim, TIM_OC1, TIM_OCM_PWM1);
    timer_set_oc_mode(tim, TIM_OC2, TIM_OCM_PWM1);
    timer_set_oc_mode(tim, TIM_OC3, TIM_OCM_PWM1);
    timer_set_oc_mode(tim, TIM_OC4, TIM_OCM_PWM1);
    timer_enable_oc_output(tim, TIM_OC1);
    timer_enable_oc_output(tim, TIM_OC2);
    timer_enable_oc_output(tim, TIM_OC3);
    timer_enable_oc_output(tim, TIM_OC4);
    timer_enable_break_main_output(tim);
    timer_enable_counter(tim);

    /*rcc_periph_clock_enable(RCC_TIM2);
    TIM2_CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E|TIM_CCER_CC4E; //
    ENABLE TIM2_CCMR1 |= TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC2M_PWM1; TIM2_CCMR2
    |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC4M_PWM1; TIM2_CR1 = TIM_CR1_CEN;*/
    /*rcc_periph_clock_enable(RCC_TIM4);
    TIM4_CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E|TIM_CCER_CC4E; //
    ENABLE TIM4_CCMR1 |= TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC2M_PWM1; TIM4_CCMR2
    |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC4M_PWM1; TIM4_CR1 = TIM_CR1_CEN;*/

    //
    //		for(int PWM_pulse = 0 ; PWM_pulse < 65535; PWM_pulse += 100) {
    //
    //			/*timer_set_oc_value(TIM2, TIM_OC1 , PWM_pulse);
    //			timer_set_oc_value(TIM2, TIM_OC2 , PWM_pulse);
    //			timer_set_oc_value(TIM2, TIM_OC3 , PWM_pulse);
    //			timer_set_oc_value(TIM2, TIM_OC4 , PWM_pulse);*/
    //
    //			timer_set_oc_value(TIM4, TIM_OC1 , PWM_pulse);
    //			timer_set_oc_value(TIM4, TIM_OC2 , PWM_pulse);
    //			timer_set_oc_value(TIM4, TIM_OC3 , PWM_pulse);
    //			timer_set_oc_value(TIM4, TIM_OC4 , PWM_pulse);
    //			for (int i = 0; i < 800; i++);
    //		}
    //		for(int PWM_pulse = 65535 ; PWM_pulse > 0; PWM_pulse -=  100) {
    //			/*timer_set_oc_value(TIM2, TIM_OC1 , PWM_pulse);
    //			timer_set_oc_value(TIM2, TIM_OC2 , PWM_pulse);
    //			timer_set_oc_value(TIM2, TIM_OC3 , PWM_pulse);
    //			timer_set_oc_value(TIM2, TIM_OC4 , PWM_pulse);*/
    //
    //			timer_set_oc_value(TIM4, TIM_OC1 , PWM_pulse);
    //			timer_set_oc_value(TIM4, TIM_OC2 , PWM_pulse);
    //			timer_set_oc_value(TIM4, TIM_OC3 , PWM_pulse);
    //			timer_set_oc_value(TIM4, TIM_OC4 , PWM_pulse);
    //			for (int i = 0; i < 800; i++);
    //		}
}

void RemoteModule::_usart_setup()
{  // ÍÅ ÂÛÇÛÂÀÅÒÑß
    /* Enable clocks for GPIO port A (for GPIO_USART1_TX) and USART1. */
    rcc_periph_clock_enable(RCC_USART1);

    /* Setup GPIO pin GPIO_USART1_TX/GPIO9 on GPIO port A for transmit. */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

    /* Setup UART parameters. */
    usart_set_baudrate(USART1, 115200);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

    /* Finally enable the USART. */
    usart_enable(USART1);
}
