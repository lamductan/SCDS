#ifndef SCDS_ALGORITHMS_IALG_NODE_H_
#define SCDS_ALGORITHMS_IALG_NODE_H_

#include <vector>
#include <set>
#include <omnetpp.h>

#include "algorithms/ialg.h"
#include "node/Node.h"

using namespace omnetpp;

enum MISNodeStatus {
    UNDECIDED,
    IN_MIS,
    NOT_IN_MIS
};

enum CDSNodeStatus {
    NOT_IN_CDS,
    IN_CDS
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
public:
    Node *node;
    int id;
    MISNodeStatus MIS_status = UNDECIDED;
    MISNodeStatus previous_MIS_status = UNDECIDED;
    CDSNodeStatus CDS_status = NOT_IN_CDS;
    CDSNodeStatus previous_CDS_status = NOT_IN_CDS;
    std::vector<cMessage *> message_queue;
    std::set<int> need_to_send;
    int current_round_alg_stage = BaseAlgStage::INITIAL_STAGE;
    int previous_round_alg_stage = BaseAlgStage::INITIAL_STAGE;

    void init(Node *node, int starting_round);
    IAlgNode();
    IAlgNode(Node *node, int starting_round=1);
    
    int n_awake_rounds = 0;
    int n_sleep_rounds = 0;
    int last_communication_round = -1;
    int decided_round = -1;
    std::map<int,bool> awake_round_map;

    int dominator = -1;
    
    virtual bool is_selected(); //should be overriden

    virtual bool is_awake(); //should be overriden
    virtual bool is_decided(); //should be overriden
    
    virtual void handle_message(cMessage *msg) override; //should NOT be overriden
    virtual void start_round(cMessage *msg); //should NOT be overriden
    virtual void process_round(); //may be overriden, but UNLIKELY
    virtual void stage_transition(); //should be overriden
    virtual cMessage *process_message_queue();  //should be overriden
    virtual void clear_message_queue(); //should NOT be overriden
    virtual void send_new_message(cMessage *msg, double delay=0.5); //should NOT be overriden
    virtual void listen_new_message(cMessage *msg); //should NOT be overriden

    virtual void record_decided_round();
    virtual void update_previous_status();

    virtual void call_handle_message(IAlgNode *alg, cMessage *msg);

    virtual ~IAlgNode();
};

#endif //SCDS_ALGORITHMS_IALG_NODE_H_