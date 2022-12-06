#ifndef SCDS_ALGORITHMS_MST_AMP22_MST_AMP22_MST_H_
#define SCDS_ALGORITHMS_MST_AMP22_MST_AMP22_MST_H_

#include "algorithms/ialg_node.h"
#include "algorithms/mst/AMP22_mst/AMP22_mst_message.h"

class AMP22MSTAlg : public IAlgNode
{
  public:
    std::set<int> all_neighbors_set;
    int d = -1;
    int phase_id = -1;
    int log2_n = -1;
    int current_phase_starting_round = -1;
    int next_phase_starting_round = -1;
    int round_id_in_current_phase = -1;
    int n_rounds_in_current_phase = -1;

    std::map<int, AMP22MSTRoundPhaseActionType> awake_rounds_in_phase;
    std::map<AMP22MSTRoundPhaseActionType, int> awake_round_type_to_round_id;
    bool is_endpoint_moe = false;

    std::map<int, centralized::Edge> edges_map;
    std::map<int, std::tuple<int, int, int>> edges_id_map;

    int fragment_id = -1;
    int level = -1;
    int parent = -1;
    std::set<int> children;

    std::tuple<int, int, int> fragment_moe = INF_EDGE_ID;

    std::set<int> different_fragment_neighbors;
    std::set<int> same_fragment_neighbors;
    std::set<std::tuple<int, int, int>> basic_edges_set;
    AMP22MSTCoinValue coin_value = AMP22_MST_COIN_VALUE_UNDEFINED;
    std::tuple<int, int, int> node_moe = INF_EDGE_ID;

    int new_fragment_id = -1;
    int new_level = -1;
    int new_parent = -2;
    std::set<int> new_chilren;
    bool updated_fragment_information = false;

  public:
    AMP22MSTMessage *create_AMP22_MST_message(int real_receiver_id = -2);

    void update_new_fragment_information_from_msg(
        AMP22MSTMessage *AMP22_MST_message);
    void print_message_queue();

    AMP22MSTCoinValue toss_coin() const;

    virtual int calc_n_rounds_in_current_phase() const;
    int calc_round_id_in_current_phase() const;
    virtual void reset_phase();
    void reset_phase_rounds_information();
    bool is_fragment_leaf() const;
    bool is_fragment_root() const;
    virtual int get_round_stretch() const;
    void add_awake_round(int round_id, AMP22MSTRoundPhaseActionType round_type);
    void remove_awake_round(AMP22MSTRoundPhaseActionType round_type);
    int get_new_level(int new_parent_level) const;

    AMP22MSTMessage *process_message_queue_UPCAST_SEND_MOE_round();
    AMP22MSTMessage *process_message_queue_BROADCAST_SEND_MOE_round();

    AMP22MSTMessage *
    process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_1_round();
    AMP22MSTMessage *
    process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_1_2_round();
    AMP22MSTMessage *
    process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_2_round();

    AMP22MSTMessage *process_message_queue_UPCAST_SEND_TAIL_round();
    AMP22MSTMessage *process_message_queue_BROADCAST_SEND_TAIL_round();

    AMP22MSTMessage *
    process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_1_round();
    AMP22MSTMessage *
    process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_1_2_round();
    AMP22MSTMessage *
    process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_2_round();

    AMP22MSTMessage *process_message_queue_BROADCAST_SEND_MOE_round_root();
    AMP22MSTMessage *process_message_queue_BROADCAST_SEND_MOE_round_non_root();

  public:
    virtual void set_alg_type() override;
    virtual void init(Node *node, int starting_round) override;
    AMP22MSTAlg();
    AMP22MSTAlg(Node *node, int starting_round = 1);
    virtual void stage_transition() override;
    virtual cMessage *process_message_queue() override;
    AMP22MSTMessage *process_message_queue_1();
    virtual void send_new_message(cMessage *msg,
                                  double delay = DELAY_SENT) override;
    virtual bool is_selected() override;
    virtual bool is_awake() override;
    virtual bool is_decided() override;
    virtual void print_state(int log_level = 0) override;
    virtual int find_next_relay_node_id_on_path_to(int real_receiver_id);
};

std::string map_AMP22MSTRoundPhaseActionType_to_string(
    const std::map<int, AMP22MSTRoundPhaseActionType> &m, int log_level = 0,
    bool log_level_on_first_line = true);

#endif // SCDS_ALGORITHMS_MST_AMP22_MST_AMP22_MST_H_