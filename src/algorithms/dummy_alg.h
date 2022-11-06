#ifndef SCDS_ALGORITHMS_DUMMY_ALG_H_
#define SCDS_ALGORITHMS_DUMMY_ALG_H_

#include <omnetpp.h>

#include "algorithms/ialg.h"
#include "networks/Network.h"
#include "node/Node.h"

using namespace omnetpp;

class DummyAlgNetwork : public IAlg
{
protected:
    Network *network;

public:
    DummyAlgNetwork(Network *network);
    void start() override;
    void handle_message(cMessage *msg) override;
    virtual ~DummyAlgNetwork();
};


class DummyAlgNode : public IAlg
{
protected:
    Node *node;
public:
    DummyAlgNode(Node *node);
    void start() override;
    void handle_message(cMessage *msg) override;
    virtual ~DummyAlgNode();
};

#endif //SCDS_ALGORITHMS_DUMMY_ALG_H_