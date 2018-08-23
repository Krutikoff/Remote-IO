#include "i2c_module.h"

void I2cModule::run()
{
    if (!_queue.empty()) _queue.clear();
}
