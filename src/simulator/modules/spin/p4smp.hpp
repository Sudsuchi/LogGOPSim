#ifndef __P4_SMP_HPP__
#define __P4_SMP_HPP__

#include "../../../config.h"

#ifdef HAVE_GEM5


#include "../portals4/p4.hpp"
#include "gem5/GEM5Mod.hpp"
#include "p4smp_events.hpp"

#include <unordered_map>
#include <queue>







class P4SMPMod: public P4Mod {

private:
    uint32_t c_packet;
    GEM5Mod * gem5;
    
public:
    P4SMPMod(Simulator& sim, Parser& parser, bool simplenet=true): P4Mod(sim, parser, simplenet) {


    

        if (sim.args_info.gem5_conf_file_given && simplenet){
            printf("Warning: cannot use gem5 with simplenet (ignoring gem5 option)\n");
        }else{
            c_packet = sim.args_info.sPIN_c_packet_arg;// 0;
        }

    }

    ~P4SMPMod(){
        if (gem5!=NULL) delete gem5;
    }


    inline int recvpkt(HostDataPkt& pkt);
    inline int processHandlers(MatchedHostDataPkt& pkt);
    static int dispatch(SimModule* mod, simevent* ev);

    virtual int registerHandlers(Simulator& sim){
        P4Mod::registerHandlers(sim);

        if (gem5!=NULL){
            sim.addHandler(this, HOST_DATA_PKT, P4SMPMod::dispatch);
            sim.addHandler(this, MATCHED_HOST_DATA_PKT, P4SMPMod::dispatch);
        }

        return 0;
    }


    void erase_matching_entry(uint32_t host, uint32_t header_id);
    void reset_matching_entry(uint32_t host, uint32_t header_id);


    virtual void printStatus();
    virtual size_t maxTime();


};

#endif
#endif /* __P4_SMP_HPP__ */
