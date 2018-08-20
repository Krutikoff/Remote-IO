#include "modbus.h"
#include <algorithm>

USHORT Modbus::usRegCoilStart = REG_COIL_START;
BOOL   Modbus::usRegCoilBuf[REG_COIL_NREGS] = {0};

USHORT Modbus::usRegDiscreteStart = REG_DISCRETE_START;
BOOL   Modbus::usRegDiscreteBuf[REG_DISCRETE_NREGS] = { 0, 0, 0, 0, 0, 0, 0, 0,
														1, 1, 1, 1, 1, 1, 1 ,1,
														0, 0, 0, 0, 0, 0, 0, 0 };

USHORT Modbus::usRegHoldingStart = REG_HOLDING_START;
USHORT Modbus::usRegHoldingBuf[REG_HOLDING_NREGS] = {0};

USHORT Modbus::usRegInputStart = REG_INPUT_START;
USHORT Modbus::usRegInputBuf[REG_INPUT_NREGS] = {0};





Modbus *_modbus_ptr;

uint8_t* Modbus::coils_it_counter_clear = _modbus_ptr->get_iterator <uint8_t>(0x0012);

uint8_t* Modbus::discrete_it_read = _modbus_ptr->get_iterator <uint8_t>(0x1001);
uint8_t* Modbus::discrete_it_latch_low = _modbus_ptr->get_iterator <uint8_t>(0x1009);
uint8_t* Modbus::discrete_it_latch_high = _modbus_ptr->get_iterator <uint8_t>(0x1011);

uint16_t* Modbus::input_it_counter = _modbus_ptr->get_iterator <uint16_t>(0x3001);

uint32_t* Modbus::holding_it_baudrate = _modbus_ptr->get_iterator <uint32_t>(0x4001);


Modbus::Modbus() {

	_modbus_ptr = this;

	eMBErrorCode status;

	modbus_address_device = (UCHAR)((~( (gpio_port_read(GPIOA) >> 4) ) ) & 0x000F);

	status = eMBInit(eMBMode::MB_RTU, modbus_address_device, 0, MODBUS_BAUDRATE, MB_PAR_NONE);
	if(status != eMBErrorCode::MB_ENOERR) {
			__asm("nop");
	}

	status = eMBEnable(  );
	if(status != eMBErrorCode::MB_ENOERR) {
				__asm("nop");
	}

}



void Modbus::poll()
{
	eMBErrorCode status;
	status = eMBPoll();
	if(status != eMBErrorCode::MB_ENOERR) {
		__asm("nop");
	}
	__asm("nop");

}

void Modbus::set_register(RegisterType type, uint32_t address, uint16_t value)
{
	if ( (type == COIL) && (address < REG_COIL_NREGS) ) {
		usRegCoilBuf[address] = value;
	}
	else if ( (type == DISCRETE) && (address < REG_DISCRETE_NREGS) ) {
		usRegDiscreteBuf[address] = value;
	}
	else if((type == HOLDING) && (address < REG_HOLDING_NREGS)) {
		usRegHoldingBuf[address] = value;
	}
	else if((type == INPUT) && (address < REG_INPUT_NREGS)) {
		usRegInputBuf[address] = value;
	}
}



Modbus::Error Modbus::get_register(RegisterType type, uint32_t address, uint16_t &value)
{

	if ( (type == COIL) && (address < REG_COIL_NREGS) ) {
		value = usRegCoilBuf[address];
		return Error::MB_ENOERR;
	}
	else if ( (type == DISCRETE) && (address < REG_DISCRETE_NREGS) ) {
		value = usRegDiscreteBuf[address];
		return Error::MB_ENOERR;
	}
	else if((type == HOLDING)  && (address < REG_HOLDING_NREGS)) {
		value = usRegHoldingBuf[address];
		return Error::MB_ENOERR;
	}
	else if((type == INPUT)  && (address < REG_INPUT_NREGS)) {
		value = usRegInputBuf[address];
		return Error::MB_ENOERR;
	}
	return Error::MB_ENOERR;
}

RegisterType Modbus::_get_type(uint32_t address){
	RegisterType type;

	if(address >= REG_COIL_START && address <= REG_COIL_START + REG_COIL_NREGS){
		return type = RegisterType::COIL;
	}
	else if(address >= REG_DISCRETE_START && address <= REG_DISCRETE_START + REG_DISCRETE_NREGS) {
		return type = RegisterType::DISCRETE;
	}
	else if(address >= REG_HOLDING_START && address <= REG_HOLDING_START + REG_HOLDING_NREGS){
		return type = RegisterType::HOLDING;
	}
	else if(address >= REG_INPUT_START && address <= REG_INPUT_START + REG_INPUT_NREGS){
		return type = RegisterType::INPUT;
	}
	else
		return RegisterType::ERROR;
}



void Modbus::mb_gpio_set(uint8_t* coil_data_set) {

	if(coil_data_set[0]){

		exti_disable_request(EXTI0); // Запрещаем прерывания для чтения данных
		config_write[0] = true;
		config_read[0] = false;
	}
	else {
		exti_enable_request(EXTI0); // Разрешаем прерывания для чтения данных
		config_write[0] = false;
		config_read[0] = true;
	}

	if(coil_data_set[1]){
		exti_disable_request(EXTI1);
		config_write[1] = true;
		config_read[1] = false;
	}
	else {
		exti_enable_request(EXTI1); // Разрешаем прерывания для чтения данных
		config_write[1] = false;
		config_read[1] = true;
	}

	if(coil_data_set[2]){
		exti_disable_request(EXTI2);
		config_write[2] = true;
		config_read[2] = false;
	}
	else {
		exti_enable_request(EXTI2); // Разрешаем прерывания для чтения данных
		config_write[2] = false;
		config_read[2] = true;
	}
	if(coil_data_set[3]){
		exti_disable_request(EXTI3);
		config_write[3] = true;
		config_read[3] = false;
	}
	else {
		exti_enable_request(EXTI3); // Разрешаем прерывания для чтения данных
		config_write[3] = false;
		config_read[3] = true;
	}
	if(coil_data_set[4]){
		exti_disable_request(EXTI4);
		config_write[4] = true;
		config_read[4] = false;
	}
	else{
		exti_enable_request(EXTI4); // Разрешаем прерывания для чтения данных
		config_write[4] = false;
		config_read[4] = true;
	}
	if(coil_data_set[5]){
		exti_disable_request(EXTI5);
		config_write[5] = true;
		config_read[5] = false;
	}
	else{
		exti_enable_request(EXTI5); // Разрешаем прерывания для чтения данных
		config_write[5] = false;
		config_read[5] = true;
	}
	if(coil_data_set[6]){
		exti_disable_request(EXTI6);
		config_write[6] = true;
		config_read[6] = false;
	}
	else {
		exti_enable_request(EXTI6); // Разрешаем прерывания для чтения данных
		config_write[6] = false;
		config_read[6] = true;
	}
	if(coil_data_set[7]){
		exti_disable_request(EXTI7);
		config_write[7] = true;
		config_read[7] = false;
	}
	else {
		exti_enable_request(EXTI7); // Разрешаем прерывания для чтения данных
		config_write[7] = false;
		config_read[7] = true;
	}
}

void Modbus::mb_gpio_write(uint8_t* coil_data_write) {

	if(config_write[0]) {



		if(*coil_data_write){
			gpio_set(GPIOA, GPIO0);
		}
		else
			gpio_clear(GPIOA, GPIO0);


	}

	if(config_write[1]) {
		if(*(coil_data_write + 1)){
			gpio_set(GPIOA, GPIO1);
		}
		else
			gpio_clear(GPIOA, GPIO1);


	}

	if(config_write[2]) {

		if(*(coil_data_write + 2)){
			gpio_set(GPIOA, GPIO2);
		}
		else
			gpio_clear(GPIOA, GPIO2);


	}

	if(config_write[3]) {
		if(*(coil_data_write + 3)){
			gpio_set(GPIOA, GPIO3);
		}
		else
			gpio_clear(GPIOA, GPIO3);


	}

	if(config_write[4]) {
		if(*(coil_data_write + 4)){
			gpio_set(GPIOB, GPIO6);
		}
		else
			gpio_clear(GPIOB, GPIO6);


	}

	if(config_write[5]) {
		if(*(coil_data_write + 5)){
			gpio_set(GPIOB, GPIO7);
		}
		else
			gpio_clear(GPIOB, GPIO7);


	}

	if(config_write[6]) {
		if(*(coil_data_write + 6)){
			gpio_set(GPIOB, GPIO8);
		}
		else
			gpio_clear(GPIOB, GPIO8);


	}

	if(config_write[7]) {
		if(*(coil_data_write + 7)){
			gpio_set(GPIOB, GPIO9);
		}
		else
			gpio_clear(GPIOB, GPIO9);


	}
}

void mb_latch_low(uint8_t* input_latch_low) {

	*input_latch_low = gpio_get(GPIOC, GPIO0);
	*(input_latch_low + 1) = gpio_get(GPIOC, GPIO1);
	*(input_latch_low + 2) = gpio_get(GPIOC, GPIO2);
	*(input_latch_low + 3) = gpio_get(GPIOC, GPIO3);
	*(input_latch_low + 4) = gpio_get(GPIOC, GPIO4);
	*(input_latch_low + 5) = gpio_get(GPIOC, GPIO5);
	*(input_latch_low + 6) = gpio_get(GPIOC, GPIO6);
	*(input_latch_low + 7) = gpio_get(GPIOC, GPIO7);
}

void Modbus::mb_latch_high(uint8_t* input_latch_high) {

}

void Modbus::mb_latch_clear() {



	uint8_t clear_low[8] = {1, 1, 1, 1, 1, 1, 1, 1};
	uint8_t clear_high[8] = {0, 0, 0, 0, 0, 0, 0, 0};


	std::copy_n(clear_low, 8, discrete_it_latch_low);
	std::copy_n(clear_high, 8, discrete_it_latch_high);

}

void Modbus::mb_counter_clear() {

	Modbus& modbus = Modbus::instance();

	//uint8_t clear_counter[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	//std::copy_n(clear_counter, 8, input_it_counter);

	if(coils_it_counter_clear[0]){
		modbus.input_it_counter[0] = 0;
		coils_it_counter_clear[0] = 0;
	}

	if(coils_it_counter_clear[1]) {
		modbus.input_it_counter[1] = 0;
		coils_it_counter_clear[1] = 0;
	}
	if(coils_it_counter_clear[2]){
		modbus.input_it_counter[2] = 0;
		coils_it_counter_clear[2] = 0;
	}

	if(coils_it_counter_clear[3]){
		modbus.input_it_counter[3] = 0;
		coils_it_counter_clear[3] = 0;
	}

	if(coils_it_counter_clear[4]){
		modbus.input_it_counter[4] = 0;
		coils_it_counter_clear[4] = 0;
	}

	if(coils_it_counter_clear[5]){
		modbus.input_it_counter[5] = 0;
		coils_it_counter_clear[5] = 0;
	}

	if(coils_it_counter_clear[6]){
		modbus.input_it_counter[6] = 0;
		coils_it_counter_clear[6] = 0;
	}

	if(coils_it_counter_clear[7]){
		modbus.input_it_counter[7] = 0;
		coils_it_counter_clear[7] = 0;
	}
}

void Modbus::mb_baudrate_set(){

	Modbus& modbus = Modbus::instance();

	if(modbus.baudrate != *modbus.holding_it_baudrate) {
		modbus.baudrate = *modbus.holding_it_baudrate;

		eMBErrorCode status;

		status = eMBInit(eMBMode::MB_RTU, modbus_address_device, 0, modbus.baudrate, MB_PAR_NONE);
		if(status != eMBErrorCode::MB_ENOERR) {
				__asm("nop");
		}

		status = eMBEnable(  );
		if(status != eMBErrorCode::MB_ENOERR) {
					__asm("nop");
		}



	}

}

void exti0_isr(void){
	exti_reset_request(EXTI0); // Сбросили флаг

	Modbus& modbus = Modbus::instance();

	static bool latch_low_0 = false;
	static bool latch_high_0 = false;

	if(GPIOC_IDR & (GPIO0)) { // LATCH_LOW
		latch_high_0 = true;
		modbus.discrete_it_latch_low[0] = 0;
		modbus.discrete_it_latch_high[0] = 0;
	}
	else { // LATCH_HIGH
		latch_low_0 = true;
		modbus.discrete_it_latch_high[0] = 1;
		modbus.discrete_it_latch_low[0] = 1;
	}

	if(latch_high_0 && latch_low_0) {

		modbus.input_it_counter[0]++;
		if(modbus.input_it_counter[0] > 65534)
			modbus.input_it_counter[0] = 0;

		latch_low_0 = false;
		latch_high_0 = false;
	}

	modbus.discrete_it_read[0] = (uint8_t)(gpio_get(GPIOC, GPIO0)); // READ_GPIO
}


void exti1_isr(void){
	exti_reset_request(EXTI1); // Сбросили флаг

	Modbus& modbus = Modbus::instance();

	static bool latch_low_1 = false;
	static bool latch_high_1 = false;

	if(GPIOC_IDR & (GPIO1)) { // LATCH_HIGH
		//latch_high_1 = true;

		modbus.discrete_it_latch_low[1] = 0;
		modbus.discrete_it_latch_high[1] = 0;
	}
	else { // LATCH_LOW
		//latch_low_1 = true;
		modbus.discrete_it_latch_high[1] = 1;
		modbus.discrete_it_latch_low[1] = 1;

	}

	if(latch_high_1 && latch_low_1) {

		modbus.input_it_counter[1]++;
		if(modbus.input_it_counter[1] > 65534)
			modbus.input_it_counter[1] = 0;

		latch_low_1 = false;
		latch_high_1 = false;
	}

	modbus.discrete_it_read[1] = (uint8_t)(gpio_get(GPIOC, GPIO1) >> 1);
}

void exti2_isr(void){
	exti_reset_request(EXTI2); // Сбросили флаг

	Modbus& modbus = Modbus::instance();

	static bool latch_low_2 = false;
	static bool latch_high_2 = false;

	if(GPIOC_IDR & (GPIO2)) { // LATCH_HIGH
		latch_high_2 = true;

		modbus.discrete_it_latch_high[2] = 0;
		modbus.discrete_it_latch_low[2] = 0;
	}
	else { // LATCH_LOW
		latch_low_2 = true;

		modbus.discrete_it_latch_high[2] = 1;
		modbus.discrete_it_latch_low[2] = 1;
	}

	if(latch_high_2 && latch_low_2) {

		modbus.input_it_counter[2]++;
		if(modbus.input_it_counter[2] > 65534)
			modbus.input_it_counter[2] = 0;

		latch_low_2 = false;
		latch_high_2 = false;
	}

	modbus.discrete_it_read[2] = (uint8_t)(gpio_get(GPIOC, GPIO2) >> 2);
}

void exti3_isr(void){
	exti_reset_request(EXTI3); // Сбросили флаг

	Modbus& modbus = Modbus::instance();

	static bool latch_low_3 = false;
	static bool latch_high_3 = false;

	if(GPIOC_IDR & (GPIO3)) { // LATCH_HIGH
		latch_high_3 = true;

		modbus.discrete_it_latch_high[3] = 0;
		modbus.discrete_it_latch_low[3] = 0;
	}
	else { // LATCH_LOW
		latch_low_3 = true;

		modbus.discrete_it_latch_high[3] = 1;
		modbus.discrete_it_latch_low[3] = 1;
	}

	if(latch_high_3 && latch_low_3) {

		modbus.input_it_counter[3]++;
		if(modbus.input_it_counter[3] > 65534)
			modbus.input_it_counter[3] = 0;

		latch_low_3 = false;
		latch_high_3 = false;
	}

	modbus.discrete_it_read[3] = (uint8_t)(gpio_get(GPIOC, GPIO3) >> 3);
}

void exti4_isr(void){
	exti_reset_request(EXTI4); // Сбросили флаг

	Modbus& modbus = Modbus::instance();

	static bool latch_low_4 = false;
	static bool latch_high_4 = false;

	if(GPIOC_IDR & (GPIO4)) { // LATCH_HIGH
		//latch_high_4 = true;

		modbus.discrete_it_latch_high[4] = 0;
		modbus.discrete_it_latch_low[4] = 0;
	}
	else { // LATCH_LOW
		//latch_low_4 = true;

		modbus.discrete_it_latch_high[4] = 1;
		modbus.discrete_it_latch_low[4] = 1;
	}

	if(latch_high_4 && latch_low_4) {

		modbus.input_it_counter[4]++;
		if(modbus.input_it_counter[4] > 65534)
			modbus.input_it_counter[4] = 0;

		latch_low_4 = false;
		latch_high_4 = false;
	}


	modbus.discrete_it_read[4] = (uint8_t)(gpio_get(GPIOC, GPIO4) >> 4);
}

void exti9_5_isr(void){

	static bool latch_low_5 = false;
	static bool latch_high_5 = false;

	static bool latch_low_6 = false;
	static bool latch_high_6 = false;

	static bool latch_low_7 = false;
	static bool latch_high_7 = false;

	Modbus& modbus = Modbus::instance();

	if(exti_get_flag_status(EXTI5)){
		exti_reset_request(EXTI5); // Сбросили флаг


		if(gpio_get(GPIOC, GPIO5)) {
			latch_high_5 = true;

			modbus.discrete_it_latch_high[5] = 0;
			modbus.discrete_it_latch_low[5] = 0;
		}
		else{
			latch_low_5 = true;

			modbus.discrete_it_latch_high[5] = 1;
			modbus.discrete_it_latch_low[5] = 1;
		}

		if(latch_high_5 && latch_low_5) {

			modbus.input_it_counter[5]++;
			if(modbus.input_it_counter[5] > 65534)
				modbus.input_it_counter[5] = 0;

			latch_low_5 = false;
			latch_high_5 = false;
		}

		modbus.discrete_it_read[5] = (uint8_t)(gpio_get(GPIOC, GPIO5) >> 5);
		//*_modbus_ptr->discrete_it_read = gpio_get(GPIOC, GPIO5); OTHER VARIANT
	}
	else if(exti_get_flag_status(EXTI6)) {
		exti_reset_request(EXTI6);


		if(gpio_get(GPIOC, GPIO6)) {
			latch_high_6 = true;

			modbus.discrete_it_latch_high[6] = 0;
			modbus.discrete_it_latch_low[6] = 0;
		}
		else{
			latch_low_6 = true;

			modbus.discrete_it_latch_high[6] = 1;
			modbus.discrete_it_latch_low[6] = 1;
		}

		if(latch_high_6 && latch_low_6) {

			modbus.input_it_counter[6]++;
			if(modbus.input_it_counter[6] > 65534)
				modbus.input_it_counter[6] = 0;

			latch_low_6 = false;
			latch_high_6 = false;
		}

		modbus.discrete_it_read[6] = (uint8_t)(gpio_get(GPIOC, GPIO6) >> 6);
	}
	else if(exti_get_flag_status(EXTI7)) {
		exti_reset_request(EXTI7);


		if(gpio_get(GPIOC, GPIO7)) {
			latch_high_7 = true;

			modbus.discrete_it_latch_high[7] = 0;
			modbus.discrete_it_latch_low[7] = 0;
		}
		else{
			latch_low_7 = true;

			modbus.discrete_it_latch_high[7] = 1;
			modbus.discrete_it_latch_low[7] = 1;
		}

		if(latch_high_7 && latch_low_7) {

			modbus.input_it_counter[7]++;
			if(modbus.input_it_counter[7] > 65534)
				modbus.input_it_counter[7] = 0;

			latch_low_7 = false;
			latch_high_7 = false;
		}

		modbus.discrete_it_read[7] = (uint8_t)(gpio_get(GPIOC, GPIO7) >> 7);

	}

}



/**************************************************************/
/**********************  MODBUS FUNCTION CODE ****************************/
/**************************************************************/

eMBErrorCode    eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress,
                               USHORT usNRegs ) {
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;

	Modbus& modbus = Modbus::instance();
	if( ( usAddress >= modbus.REG_INPUT_START )
		&& ( usAddress + usNRegs <= modbus.REG_INPUT_START + modbus.REG_INPUT_NREGS ) )
	{
		iRegIndex = ( int )( usAddress - modbus.usRegInputStart );
		while( usNRegs > 0 )
		{
			*pucRegBuffer++ = ( unsigned char )( modbus.usRegInputBuf[iRegIndex] >> 8 );
			*pucRegBuffer++ = ( unsigned char )( modbus.usRegInputBuf[iRegIndex] & 0xFF );
			iRegIndex++;
			usNRegs--;
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

eMBErrorCode    eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress,
                                 USHORT usNRegs, eMBRegisterMode eMode ) {
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;


	Modbus& modbus = Modbus::instance();
	if( ( usAddress >= modbus.REG_HOLDING_START ) && ( usAddress + usNRegs <= modbus.REG_HOLDING_START + modbus.REG_HOLDING_NREGS ) )
	{
		iRegIndex = ( int )( usAddress - modbus.usRegHoldingStart );
		switch ( eMode )
		{
		case MB_REG_READ:
			while( usNRegs > 0 )
			{
				*pucRegBuffer++ = ( unsigned char )( modbus.usRegHoldingBuf[iRegIndex] >> 8 );
				*pucRegBuffer++ = ( unsigned char )( modbus.usRegHoldingBuf[iRegIndex] & 0xFF );
				iRegIndex++;
				usNRegs--;
			}
			break;

		case MB_REG_WRITE:
			while( usNRegs > 0 )
			{
				modbus.usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
				modbus.usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
				iRegIndex++;
				usNRegs--;
			}
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

eMBErrorCode    eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress,
                               USHORT usNCoils, eMBRegisterMode eMode ) {
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;

	Modbus& modbus = Modbus::instance();
	if( ( usAddress >= modbus.REG_COIL_START ) && ( usAddress + usNCoils <= modbus.REG_COIL_START + modbus.REG_COIL_NREGS ) )
	{
		iRegIndex = ( int )( usAddress - modbus.usRegCoilStart );
		switch ( eMode )
		{
		case MB_REG_READ:
			while( usNCoils > 0 )
			{
				*pucRegBuffer++ = (unsigned char)(modbus.usRegCoilBuf[iRegIndex]);
				iRegIndex++;
				usNCoils--;
			}
			break;

		case MB_REG_WRITE:
			while( usNCoils > 0 )
			{
				modbus.usRegCoilBuf[iRegIndex] = *pucRegBuffer++;
				iRegIndex++;
				usNCoils--;
			}
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

eMBErrorCode    eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress,
                                  USHORT usNDiscrete ) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    Modbus& modbus = Modbus::instance();
    if( ( usAddress >= modbus.REG_DISCRETE_START )
        && ( usAddress + usNDiscrete <= modbus.REG_DISCRETE_START + modbus.REG_DISCRETE_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - modbus.usRegDiscreteStart );
        while( usNDiscrete > 0 )
        {
        	*pucRegBuffer++ = (unsigned char)(modbus.usRegDiscreteBuf[iRegIndex]);
            //*pucRegBuffer++ = ( unsigned char )( usRegDiscreteBuf[iRegIndex] >> 8 );
            //*pucRegBuffer++ = ( unsigned char )( usRegDiscreteBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNDiscrete--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}





