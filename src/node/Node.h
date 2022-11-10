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
    std::unordered_map<int, cGate *> neighbor_gates;
    std::vector<int> all_neighbors;
    void connect(cGate *src, cGate *dest, int destid);
    IAlgNode *alg;
    virtual ~Node();
};

Define_Module(Node);

#endif //SCDS_NODE_NODE_H_