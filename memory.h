#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <cstring>

using namespace sc_core;
using namespace tlm;
using namespace std;

enum HexRecordType : uint8_t {
    DATA_RECORD              = 0x00,
    END_OF_FILE              = 0x01,
    EXTENDED_SEGMENT_ADDRESS = 0x02,
    START_SEGMENT_ADDRESS    = 0x03,
    EXTENDED_LINEAR_ADDRESS  = 0x04,
    START_LINEAR_ADDRESS     = 0x05
};

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
            uint32_t value = mem[addr] | (mem[addr + 1] << 8) | (mem[addr + 2] << 16) |(mem[addr + 3] << 24);
            *(uint32_t*)data = value;
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

    bool load_hex(const string& filename){
        ifstream file(filename);
        if(!file.is_open()){
            cout<<"[MEM] Can not open hex file"<<endl;
            return false;
        }

        string line;
        uint32_t base_addr = 0;

        while(getline(file, line)){
            if(line[0] != ':'){
                cout<<"[MEM] Invalid hex file"<<endl;
                return false;               
            }

            //seperate hex file
            auto hex_tmp = [&line](int pos, int len){
                return stoul(line.substr(pos,len), nullptr, 16);
            };

            uint8_t len = hex_tmp(1,2);
            uint16_t addr = hex_tmp(3,4);
            HexRecordType type = static_cast<HexRecordType>(hex_tmp(7, 2));

            //uint8_t checksum = len + (addr>>8) + (addr&0xFF) + type;
            switch (type) {
                case DATA_RECORD: { //data record
                    for(int i=0;i<len;i++){
                        uint8_t data = hex_tmp(9+i*2,2);
                        uint32_t mem_addr = base_addr + addr + i;
                        if(mem_addr>=sizeof(mem)){
                            cout<<"[MEM] Address out of range: 0x"<<hex<<mem_addr<<endl;
                            return false;
                        }
                        mem[mem_addr] = data;
                    }
                    break;
                }

                case END_OF_FILE: {
                    cout<<"[MEM] End of file on HEX"<<endl;
                    break;
                }

                case EXTENDED_LINEAR_ADDRESS: {
                    uint16_t extended = hex_tmp(9,4);
                    base_addr = static_cast<uint32_t>(extended) << 16;
                    break;                    
                }

                default:
                    //do nothing
                    break;
            }
        }
        cout << "[MEM] HEX loaded to CPU successfully\n";
        return true;
    }
};