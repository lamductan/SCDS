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
    int cluster_id;
    int color;
    int n_nodes;
    cGate *networkWirelessIn;
    std::unordered_map<int, cGate *> neighbor_gates;
    std::vector<int> all_neighbors; //may change during the course of an algorithm
    std::vector<int> all_neighbors_original; //so it's better to save a copy
    void connect(cGate *src, cGate *dest, int destid);
    IAlgNode *alg;
    bool is_decided();
    virtual ~Node();
};

Define_Module(Node);

#endif //SCDS_NODE_NODE_H_