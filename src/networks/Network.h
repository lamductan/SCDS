#ifndef SCDS_NETWORKS_NETWORK_H_
#define SCDS_NETWORKS_NETWORK_H_

#include <cstring>
#include <fstream>
#include <iostream>
#include <omnetpp.h>
#include <unordered_map>
#include <vector>

#include "centralized/graph/graph.h"
#include "common.h"
#include "node/Node.h"

using namespace omnetpp;

class NetworkAlg;

/**
 * Builds a network dynamically, with the topology coming from a
 * text file.
 */
class Network : public IModule
{
  public:
    ModuleType getModuleType() const override { return NETWORK_TYPE; }

  protected:
    void buildNetwork(cModule *parent);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

  public:
    int n_nodes;
    std::unordered_map<int, Node *> nodes;
    std::map<Node *, cGate *> nodesWirelessIn;
    const char *network_file;
    const char *alg_name;
    NetworkAlg *alg;

    std::vector<int> get_selected_nodes();
    int get_total_awake_rounds();
    int get_max_awake_rounds();
    int get_finished_round();
    void log_result();

    std::map<int, centralized::Node *> centralized_nodes_map;
    centralized::Graph *centralized_graph;
    centralized::Graph *construct_graph();
    bool check(bool is_final_check = true);

    virtual void finish() override;
    virtual ~Network();
};

Define_Module(Network);

#endif // SCDS_NETWORKS_NETWORK_H_