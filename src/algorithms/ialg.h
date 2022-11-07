#ifndef SCDS_ALGORITHMS_IALG_H_
#define SCDS_ALGORITHMS_IALG_H_

#include <omnetpp.h>

using namespace omnetpp;

class IAlg
{
public:
    int n_nodes;
    int current_round_id = 1;
    int max_num_rounds;
    
    virtual void init(int n_nodes, const char *alg_name) {
        max_num_rounds = 2;
        if (strncmp(alg_name, "MIS", 3) == 0) {
            max_num_rounds = 10*log(n_nodes);
        } else if (strncmp(alg_name, "CDS", 3) == 0) {
            max_num_rounds = 10*n_nodes*log(n_nodes);
        } else {
            max_num_rounds = 3;
        }
    }

    virtual void handle_message(cMessage *msg) = 0;
    virtual ~IAlg() {}
};

#endif //SCDS_ALGORITHMS_IALG_H_