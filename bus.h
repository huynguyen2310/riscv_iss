#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;
using namespace tlm;
using namespace std;

struct Bus : sc_module {
    tlm_utils::simple_initiator_socket<Bus>memory_socket;
    tlm_utils::simple_target_socket<Bus>cpu_socket;
    SC_CTOR(Bus): memory_socket("memory_socket"), cpu_socket("cpu_socket"){
        cpu_socket.register_b_transport(this, &Bus::b_transport);
    }
    void b_transport(tlm_generic_payload& trans, sc_time& delay){
        memory_socket->b_transport(trans, delay);
    }
};