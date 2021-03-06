
#ifndef __VISEVENTS_HPP__
#define __VISEVENTS_HPP__

#include "../loggopsim.h"


// define vis types
static const uint32_t VIS_HOST_INST = 1;
static const uint32_t VIS_HOST_DUR = 2;
static const uint32_t VIS_HOST_FLOW = 3;
 


class visEvent{
public:
    uint32_t type;
    static uint64_t gid;

    std::string event_name; 
    std::string comment; 


    visEvent(){ 
        gid++;
        comment="";
        event_name="";
    }


    visEvent(std::string event_name): event_name(event_name){ 
        gid++;
        comment="";
    }


};



class HostInstantVisEvent: public visEvent{
public:
    uint32_t host; // id of the host . TODO: what about events generated by network?
    std::string module_name; //the name of the module which generated the event
    btime_t instanttime;
   
    HostInstantVisEvent(std::string event_name, uint32_t host, std::string module_name, btime_t time): 
    visEvent(event_name), host(host), module_name(module_name), instanttime(time)
    {
        this->type = VIS_HOST_INST;
    }
};



class HostDurationVisEvent: public visEvent{
public:
    uint32_t host; // id of the host . TODO: what about events generated by network?
    std::string module_name; //the name of the module which generated the event
    btime_t stime;   // start time
    btime_t etime;   // end time
   
    HostDurationVisEvent(std::string event_name, uint32_t host, std::string module_name, btime_t stime,btime_t etime): 
    visEvent(event_name), host(host), module_name(module_name), stime(stime), etime(etime)
    {
        this->type = VIS_HOST_DUR;
    }
};


class HostSimpleFlowVisEvent: public visEvent{
public:
    uint32_t ihost; // id of the initiator host 
    uint32_t rhost; // id of the receiver host . 
    std::string imodule_name; //the name of the initiator module
    std::string rmodule_name; //the name of the receiver module
    btime_t stime;   // start time
    btime_t etime;   // end time
   
    HostSimpleFlowVisEvent(std::string event_name, uint32_t ihost,uint32_t rhost, std::string imodule_name, std::string rmodule_name, btime_t stime,btime_t etime): 
    visEvent(event_name), ihost(ihost), rhost(rhost), imodule_name(imodule_name), rmodule_name(rmodule_name), stime(stime), etime(etime)
    {
        this->type = VIS_HOST_FLOW;
    }
};


class HostComplexFlowVisEvent: public HostSimpleFlowVisEvent{
public:
    uint32_t size; // size of message
    uint32_t G; // id of the receiver host . 

   
    HostComplexFlowVisEvent(std::string event_name, uint32_t ihost,uint32_t rhost, std::string imodule_name, std::string rmodule_name, btime_t stime,btime_t etime, int size, int G): 
    HostSimpleFlowVisEvent(event_name, ihost, rhost, imodule_name, rmodule_name, stime, etime), size(size), G(G)
    {
        this->type = VIS_HOST_FLOW;
    }
};





#endif /* __VISEVENTS_HPP__ */
