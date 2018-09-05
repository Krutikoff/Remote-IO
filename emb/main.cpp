#include <ConfigModule/remote_module.h>

int main()
{
    RemoteModule remote_module;
    Modbus& modbus = Modbus::instance();

    while (true) {
        modbus.poll();
        remote_module.run();
        //__asm__("nop");
    }

    return 0;
}
