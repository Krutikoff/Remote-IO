/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
/* ----------------------- Variables ----------------------------------------*/
//#include "tpl_std_incl.h"
#include "libopencmsis/core_cm3.h"
/* ----------------------- Start implementation -----------------------------*/
void EnterCriticalSection(void)
{
    __disable_irq();
}

void ExitCriticalSection(void)
{
    __enable_irq();
}

