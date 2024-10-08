#ifndef SCDS_NODE_NODE_H_
#define SCDS_NODE_NODE_H_

#include <cstring>
#include <omnetpp.h>
#include <unordered_map>

#include "common.h"

using namespace omnetpp;

class IAlgNode;
class NetworkAlg;

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
    std::vector<int>
        all_neighbors; // may change during the course of an algorithm
    std::vector<int> all_neighbors_original; // so it's better to save a copy
    bool is_finished = false;

  public:
    void connect(cGate *src, cGate *dest, int destid);
    IAlgNode *alg;
    NetworkAlg *network_alg;
    double threshold_ratio;
    bool is_decided();
    void callFinish() override;
    virtual ~Node();
};

Define_Module(Node);

#endif // SCDS_NODE_NODE_H_