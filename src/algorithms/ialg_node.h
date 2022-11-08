#ifndef SCDS_ALGORITHMS_IALG_NODE_H_
#define SCDS_ALGORITHMS_IALG_NODE_H_

#include <vector>
#include <unordered_map>
#include <omnetpp.h>

#include "algorithms/ialg.h"
#include "node/Node.h"

using namespace omnetpp;

enum NodeStatus {
    UNDECIDED,
    IN_MIS,
    NOT_IN_MIS,
    IN_CDS,
    NOT_IN_CDS
};


class BaseAlgStage {
protected:
    static constexpr int get_max_value() { return NEIGHBOR_DISCOVERY_STAGE; }
public:
    static const int END_STAGE = -1;
    static const int INITIAL_STAGE = 0;
    static const int NEIGHBOR_DISCOVERY_STAGE = 1;
};


class IAlgNode : public IAlg
{
protected:
    static const int LAST_NEIGHBOR_DISCOVERY_ROUND = 2;

public:
    Node *node;

    std::vector<int> all_neighbors;
    std::unordered_set<int> all_neighbors_set;
    std::unordered_map<int, cGate *> neighbor_gates;
    std::vector<cMessage *> message_queue;
    int current_round_alg_stage;
    int previous_round_alg_stage;

    void init(Node *node);
    IAlgNode();
    IAlgNode(Node *node);
    
    int n_awake_rounds = 0;
    int n_sleep_rounds = 0;
    int last_communication_round = -1;
    
    virtual bool is_selected(); //should be overriden

    virtual bool is_awake(); //should be overriden
    
    virtual void handle_message(cMessage *msg) override; //should NOT be overriden
    virtual void start_round(cMessage *msg); //should NOT be overriden
    virtual void process_round(); //may be overriden
    virtual void stage_transition(); //should be overriden
    virtual cMessage *process_message_queue();  //should be overriden
    virtual void clear_message_queue(); //should NOT be overriden
    virtual void send_new_message(cMessage *msg); //should NOT be overriden
    virtual void listen_new_message(cMessage *msg); //should NOT be overriden

    virtual ~IAlgNode() {}
};

#endif //SCDS_ALGORITHMS_IALG_NODE_H_