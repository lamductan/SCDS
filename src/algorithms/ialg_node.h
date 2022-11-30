#ifndef SCDS_ALGORITHMS_IALG_NODE_H_
#define SCDS_ALGORITHMS_IALG_NODE_H_

#include <omnetpp.h>
#include <set>
#include <vector>

#include "algorithms/ialg.h"
#include "centralized/graph/bfs_tree_structure.h"
#include "centralized/graph/edge.h"
#include "node/Node.h"

using namespace omnetpp;

const double DELAY_SENT = 0.5;

enum MISNodeStatus
{
    UNDECIDED,
    IN_MIS,
    NOT_IN_MIS
};

enum CDSNodeStatus
{
    NOT_IN_CDS,
    TENTATIVE_IN_CDS,
    IN_CDS
};

class BaseAlgStage
{
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

    virtual void init(Node *node, int starting_round);
    IAlgNode();
    IAlgNode(Node *node, int starting_round = 1);

    int n_awake_rounds = 0;
    int n_sleep_rounds = 0;
    int last_communication_round = -1;
    std::map<int, bool> awake_round_map;

    int dominator = -1;
    std::map<int, centralized::Edge> tree_edges;

    cMessage *synchronized_message_ptr = nullptr;

    int decided_round = -1;
    bool previous_decided_status = false;
    bool current_decided_status = false;

    virtual bool is_selected(); // should be overriden

    virtual bool is_awake();   // should be overriden
    virtual bool is_decided(); // should be overriden

    virtual void
    handle_message(cMessage *msg) override;    // should NOT be overriden
    virtual void start_round(cMessage *msg);   // should NOT be overriden
    virtual void process_round();              // may be overriden, but UNLIKELY
    virtual void stage_transition();           // should be overriden
    virtual cMessage *process_message_queue(); // should be overriden
    virtual void clear_message_queue();        // should NOT be overriden
    virtual void
    send_new_message(cMessage *msg,
                     double delay = DELAY_SENT);    // should NOT be overriden
    virtual void listen_new_message(cMessage *msg); // should NOT be overriden

    virtual void record_decided_round();
    virtual void update_previous_status();

    virtual void call_handle_message(IAlgNode *alg, cMessage *msg);
    virtual void handle_synchronized_message(cMessage *msg);
    void delete_synchronized_message();

    virtual centralized::BFSTreeStructure get_bfs_tree_structure() const;

    virtual ~IAlgNode();
};

std::string MIS_status_to_string(MISNodeStatus MIS_status);
std::string CDS_status_to_string(CDSNodeStatus CDS_status);

#endif // SCDS_ALGORITHMS_IALG_NODE_H_