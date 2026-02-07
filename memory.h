#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <cstring>

using namespace sc_core;
using namespace tlm;
using namespace std;

struct Memory : sc_module {
    tlm_utils::simple_target_socket<Memory>memory_socket;
    uint32_t mem[256];
    SC_CTOR(Memory): memory_socket("memory_socket"){
        memory_socket.register_b_transport(this, &Memory::b_transport);
        memset(mem, 0, sizeof(mem));
    }
    void b_transport(tlm_generic_payload& trans, sc_time& delay){
        uint32_t addr = trans.get_address();
        uint32_t *data = (uint32_t*)trans.get_data_ptr();

        if(addr >= sizeof(mem)){
            trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }

        if(trans.is_read()){
            memcpy(data, &mem[addr/4], 4);
            cout<<"[MEM] READ addr=0x"<<hex<<addr<<" data=0x"<<*(uint32_t*)data<<endl;
        } else if (trans.is_write()){
            memcpy(&mem[addr/4], data, 4);
            cout<<"[MEM] WRITE addr=0x"<<hex<<addr<<" data=0x"<<*(uint32_t*)data<<endl;
        } else {
            //do nothing;
        }

        delay+=sc_time(10, SC_NS);
        trans.set_response_status(TLM_OK_RESPONSE);
    }
};