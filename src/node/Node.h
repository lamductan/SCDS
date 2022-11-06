#ifndef SCDS_NODE_NODE_H_
#define SCDS_NODE_NODE_H_

#include <unordered_map>
#include <cstring>
#include <omnetpp.h>

#include "common.h"
#include "algorithms/ialg.h"

using namespace omnetpp;

class Node : public IModule
{
public:
    ModuleType getModuleType() const override { return NODE_TYPE; }

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

public:
    cGate *networkWirelessIn;
    IAlg *alg;
    virtual ~Node();
};

Define_Module(Node);

#endif //SCDS_NODE_NODE_H_