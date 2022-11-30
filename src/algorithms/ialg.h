#ifndef SCDS_ALGORITHMS_IALG_H_
#define SCDS_ALGORITHMS_IALG_H_

#include <omnetpp.h>

using namespace omnetpp;

enum AlgType
{
    DUMMY_ALG,
    MIS_ALG,
    TWO_RS_ALG,
    CDS_ALG,
    MST_ALG,
    BFS_ALG,
    TWO_HOP_MIS_ALG
};

class IAlg
{
  public:
    int n_nodes;
    int current_round_id = 1;
    int max_num_rounds;
    int starting_round = 1;
    AlgType alg_type;

    virtual void set_alg_type();
    void set_max_num_rounds(int n_nodes);

    virtual void handle_message(cMessage *msg) = 0;
    virtual ~IAlg() {}
};

#endif // SCDS_ALGORITHMS_IALG_H_