#include <ConfigModule/remote_module.h>
//#include <Modbus/modbus.h>

int main()
{

    RemoteModule remote_module;
    //remote_module.config();


    Modbus& modbus = Modbus::instance();



    while (true) {

        modbus.poll();
        remote_module.run();
        //__asm__("nop");
    }

    return 0;
}
