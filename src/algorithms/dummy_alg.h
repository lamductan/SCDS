#ifndef SCDS_ALGORITHMS_DUMMY_ALG_H_
#define SCDS_ALGORITHMS_DUMMY_ALG_H_

#include <omnetpp.h>

#include "algorithms/ialg_node.h"
#include "node/Node.h"

using namespace omnetpp;


class DummyAlg : public IAlgNode
{
public:
    DummyAlg(Node *node, const char *alg_name);
    virtual void process_round() override;
    virtual ~DummyAlg();
};

#endif //SCDS_ALGORITHMS_DUMMY_ALG_H_