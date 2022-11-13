#ifndef SCDS_ALGORITHMS_DUMMY_ALG_H_
#define SCDS_ALGORITHMS_DUMMY_ALG_H_

#include <omnetpp.h>

#include "algorithms/ialg_node.h"
#include "node/Node.h"

using namespace omnetpp;


class DummyAlg : public IAlgNode
{
public:
    DummyAlg(Node *node, int starting_round=1);
    virtual cMessage * process_message_queue() override;
    virtual ~DummyAlg();
    virtual bool is_decided() override;
    virtual bool is_awake() override;
};

#endif //SCDS_ALGORITHMS_DUMMY_ALG_H_