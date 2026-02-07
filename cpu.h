#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <cstring>

using namespace sc_core;
using namespace tlm;
using namespace std;

struct CPU : sc_module {
    tlm_utils::simple_initiator_socket<CPU>cpu_socket;
    uint32_t regs[32];
    uint32_t pc;
    SC_CTOR(CPU): cpu_socket("cpu_socket"), pc(0) {
        memset(regs, 0, sizeof(regs));
        SC_THREAD(run);
    }

    uint32_t fetch(uint32_t addr){
        tlm_generic_payload trans;
        sc_time delay = SC_ZERO_TIME;
        uint32_t data;

        trans.set_command(TLM_READ_COMMAND);
        trans.set_address(addr);
        trans.set_data_ptr((unsigned char*)&data);
        trans.set_data_length(4);

        cout<<"[CPU] FETCH at 0x"<<hex<<addr<<endl;
        
        cpu_socket->b_transport(trans, delay);

        if(trans.get_response_status() != TLM_OK_RESPONSE){
            cout<<"[CPU] FETCH ERROR"<<endl;
        }
        wait(delay);

        return data;
    }

    void execute(uint32_t instr){
        uint32_t opcode = instr & 0x7F;
        uint32_t rd, rs1, rs2, imm;
        switch (opcode){
            case 0x13: {
                rd  = (instr >> 7) & 0x1F;
                rs1 = (instr >> 15) & 0x1F;
                imm  = (int32_t)instr >> 20;
                regs[rd] = regs[rs1] + imm;
                break;
            }
            case 0x33: {
                rd  = (instr >> 7) & 0x1F;
                rs1 = (instr >> 15) & 0x1F;
                rs2 = (instr >> 20) & 0x1F;
                regs[rd] = regs[rs1] + regs[rs2];
                break;
            }
            default:
                //do nothing
                break;
        }
        regs[0] = 0;
    }

    void run(){
        for(int i=0; i<3;i++){
            uint32_t isr = fetch(pc);
            pc+=4;
            execute(isr);
        }
        cout<<"x3 (a+b) = "<<regs[3]<<endl;
        sc_stop();
    }
};