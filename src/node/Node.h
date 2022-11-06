#ifndef SCDS_NODE_NODE_H_
#define SCDS_NODE_NODE_H_

#include <unordered_map>
#include <cstring>
#include <omnetpp.h>

#include "common.h"

using namespace omnetpp;

class IAlgNode;

class Node : public IModule
{
public:
    ModuleType getModuleType() const override { return NODE_TYPE; }

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

public:
    int id;
    int n_nodes;
    cGate *networkWirelessIn;
    IAlgNode *alg;
    virtual ~Node();
};

Define_Module(Node);

#endif //SCDS_NODE_NODE_H_