#ifndef SCDS_NETWORKS_NETWORK_H_
#define SCDS_NETWORKS_NETWORK_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <unordered_set>
#include <omnetpp.h>

#include "common.h"

using namespace omnetpp;

class NetworkAlg;

/**
 * Builds a network dynamically, with the topology coming from a
 * text file.
 */
class Network : public IModule
{
public:
    ModuleType getModuleType() const override { return NETWORK_TYPE; }

protected:
    void connect(cGate *src, cGate *dest);
    void buildNetwork(cModule *parent);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    
public:
    std::vector<cGate *> nodesWirelessIn;
    std::unordered_set<int> terminated;
    
    void send_synchrozied_message();
    simtime_t get_next_transmission_time();
    
    int n_nodes;
    int n_nodes_completed_in_current_round;
    
    NetworkAlg *alg;

    virtual ~Network();
};

Define_Module(Network);

#endif //SCDS_NETWORKS_NETWORK_H_