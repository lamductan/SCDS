#ifndef SCDS_ALGORITHMS_NETWORK_ALG_H_
#define SCDS_ALGORITHMS_NETWORK_ALG_H_

#include "algorithms/ialg.h"
#include "networks/Network.h"

using namespace omnetpp;

class NetworkAlg : public IAlg {
public:
    Network *network;
    int n_completed_round_nodes = 0;

    NetworkAlg(Network *network, const char *alg_name);
    void handle_message(cMessage *mgs);
    void send_synchronized_message();

    virtual ~NetworkAlg() {}
};

#endif //SCDS_ALGORITHMS_NETWORK_ALG_H_