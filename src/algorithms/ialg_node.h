#ifndef SCDS_ALGORITHMS_IALG_NODE_H_
#define SCDS_ALGORITHMS_IALG_NODE_H_

#include <vector>
#include <unordered_map>
#include <omnetpp.h>

#include "algorithms/ialg.h"
#include "node/Node.h"

using namespace omnetpp;

class IAlgNode : public IAlg
{
public:
    enum AlgStage {
        INITIAL_STAGE,
        NEIGHBOR_DISCOVERY_STAGE,
        MIS_STAGE,
        FINAL_STAGE
    };

    static constexpr int LAST_NEIGHBOR_DISCOVERY_ROUND = 2;

public:
    Node *node;

    std::vector<int> all_neighbors;
    std::unordered_map<int, cGate *> neighbor_gates;
    std::vector<cMessage *> message_queue;
    AlgStage alg_stage;
    
    int n_awake_rounds = 0;
    int n_sleep_rounds = 0;
    bool selected;

    bool is_selected();

    IAlgNode(Node *node, const char *alg_name);
    virtual bool is_awake();
    virtual void handle_message(cMessage *msg);
    
    virtual void start_round(cMessage *msg);
    virtual void process_round();
    virtual AlgStage stage_transition();
    virtual cMessage *process_message_queue();
    virtual void clear_message_queue();
    virtual void send_new_message(cMessage *msg);
    virtual void listen_new_message(cMessage *msg);

    virtual ~IAlgNode() {}
};

#endif //SCDS_ALGORITHMS_IALG_NODE_H_