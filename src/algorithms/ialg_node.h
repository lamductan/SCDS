#ifndef SCDS_ALGORITHMS_IALG_NODE_H_
#define SCDS_ALGORITHMS_IALG_NODE_H_

#include <omnetpp.h>

#include "algorithms/ialg.h"
#include "node/Node.h"

using namespace omnetpp;

class IAlgNode : public IAlg
{
public:
    Node *node;

    IAlgNode(Node *node, const char *alg_name);
    virtual void handle_message(cMessage *msg);
    
    virtual void start_round(cMessage *msg);
    virtual void process_round() = 0;
    virtual void end_round();

    virtual ~IAlgNode() {}
};

#endif //SCDS_ALGORITHMS_IALG_NODE_H_