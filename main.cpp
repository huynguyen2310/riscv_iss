#include "cpu.h"
#include "memory.h"
#include "bus.h"

int sc_main(int argc, char* argv[]) {
    CPU cpu("cpu");
    Bus bus("bus");
    Memory mem("mem");

    // bind sockets
    cpu.cpu_socket.bind(bus.cpu_socket);
    bus.memory_socket.bind(mem.memory_socket);

    // program: a=2, b=3, a+b
    // mem.mem[0] = 0x00200093; // addi x1,x0,2
    // mem.mem[1] = 0x00300113; // addi x2,x0,3
    // mem.mem[2] = 0x002081B3; // add  x3,x1,x2

    if(!mem.load_hex("example.hex")){
        return -1;
    }
    sc_start();
    return 0;
}