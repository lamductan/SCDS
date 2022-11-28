#ifndef SCDS_ALGORITHMS_MST_GHS_MST_H_
#define SCDS_ALGORITHMS_MST_GHS_MST_H_

#include "algorithms/ialg_node.h"
#include "algorithms/mst/GHS_mst/GHS_MST_message.h"

class GHSMSTStage : public BaseAlgStage
{
  protected:
    static constexpr int get_max_value() { return MST_STAGE; }

  public:
    static const int MST_STAGE = BaseAlgStage::get_max_value() + 1;
};

class GHSMSTAlg : public IAlgNode
{
    struct cmp
    {
        bool operator()(GHSMSTMessage *lhs, GHSMSTMessage *rhs) const
        {
            if (lhs->sent_round_id < rhs->sent_round_id)
                return true;
            if (lhs->sent_round_id == rhs->sent_round_id &&
                lhs->sender_id < rhs->sender_id)
                return true;
            if (lhs->sent_round_id == rhs->sent_round_id &&
                lhs->sender_id == rhs->sender_id &&
                lhs->receiver_id < rhs->receiver_id)
                return true;
            if (lhs->sent_round_id == rhs->sent_round_id &&
                lhs->sender_id == rhs->sender_id &&
                lhs->receiver_id == rhs->receiver_id &&
                lhs->message_id < rhs->message_id)
                return true;
            return false;
        }
    };

  public:
    int parent;
    std::set<int> children;
    int level = 0;
    GHSMSTNodeState GHS_MST_node_state = GHS_MST_NODE_STATE_FOUND;
    int find_count = 0;
    std::tuple<int, int, int> minimum_outgoing_edge_id;
    std::map<std::tuple<int, int, int>, centralized::Edge> edges_map;
    std::pair<std::tuple<int, int, int>, int> fragment_core;
    std::map<std::tuple<int, int, int>, GHSMSTEdgeState> edges_state_map;
    std::set<std::tuple<int, int, int>> basic_edges_set;
    std::set<std::tuple<int, int, int>> branch_edges_set;
    std::tuple<int, int, int> best_edge_id;
    std::tuple<int, int, int> test_edge_id;
    std::tuple<int, int, int> in_branch;
    int received_best_edge_from = -1;

    int message_id = 0;

    std::set<GHSMSTMessage *, cmp> async_message_queue;
    std::set<GHSMSTMessage *, cmp> need_to_send_message_queue;

    virtual GHSMSTMessage *
    create_message(GHSMSTMessageType GHS_MST_message_type,
                   const std::tuple<int, int, int> &edge_id,
                   int real_sender_id = -1, int real_receiver_id = -2,
                   int prev_node_on_path_id = -1,
                   int next_node_on_path_id = -1);

  public:
    virtual void set_alg_type() override;
    virtual void init(Node *node, int starting_round) override;
    GHSMSTAlg();
    GHSMSTAlg(Node *node, int starting_round = 1);
    virtual void process_round() override;
    virtual void process_async_message_queue();
    virtual void send_messages_current_round();
    virtual void listen_new_message(cMessage *msg) override;
    virtual void clear_message_queue() override;
    virtual bool is_decided() override;
    virtual bool is_awake() override;
    virtual void record_decided_round() override;

    int get_neighbor_id(const std::tuple<int, int, int> &edge_id) const;
    bool is_endpoint_of_edge(const std::tuple<int, int, int> &edge_id) const;
    virtual int find_next_relay_node_id_on_path_to(int next_node_on_path_id);

    void add_branch_edge(const std::tuple<int, int, int> &edge_id);
    void add_rejected_edge(const std::tuple<int, int, int> &edge_id);

    bool handle_connect_message(GHSMSTMessage *msg);
    bool handle_initiate_message(GHSMSTMessage *msg);
    bool handle_test_message(GHSMSTMessage *msg);
    bool handle_accept_message(GHSMSTMessage *msg);
    bool handle_reject_message(GHSMSTMessage *msg);
    bool handle_report_message(GHSMSTMessage *msg);
    bool handle_change_root_message(GHSMSTMessage *msg);
    bool handle_done_message(GHSMSTMessage *msg);

    void update_level_and_fragment_core(
        int new_level,
        const std::pair<std::tuple<int, int, int>, int> &new_fragment_core);
    void test();
    void report();
    void change_root(int core_node_id);
    void print_state(int log_level = 0) const;
    virtual ~GHSMSTAlg();
};

#endif // SCDS_ALGORITHMS_MST_GHS_MST_H_