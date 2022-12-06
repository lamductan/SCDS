#ifndef SCDS_ALGORITHMS_CDS_LAM23_CDS_CONGEST_THREE_HOP_AMP22_MST_H_
#define SCDS_ALGORITHMS_CDS_LAM23_CDS_CONGEST_THREE_HOP_AMP22_MST_H_

#include "algorithms/mst/AMP22_mst/AMP22_mst.h"

class ThreeHopAMP22MSTAlg : public AMP22MSTAlg
{
    struct cmp
    {
        bool operator()(AMP22MSTMessage *lhs, AMP22MSTMessage *rhs) const
        {
            if (lhs->sent_round_id < rhs->sent_round_id) return true;
            if (lhs->sent_round_id == rhs->sent_round_id &&
                lhs->real_sender_id < rhs->real_sender_id)
                return true;
            if (lhs->sent_round_id == rhs->sent_round_id &&
                lhs->real_sender_id == rhs->real_sender_id &&
                lhs->real_receiver_id < rhs->real_receiver_id)
                return true;
            return false;
        }
    };

    struct FragmentInformation
    {
        int fragment_id;
        int level;
        int parent;
        AMP22MSTCoinValue coin_value = AMP22_MST_COIN_VALUE_UNDEFINED;

        FragmentInformation() {}
        FragmentInformation(int fragment_id, int level, int parent)
            : fragment_id(fragment_id), level(level), parent(parent)
        {
        }
    };

  public:
    std::map<int, std::array<int, 4>> nodes_on_path;
    std::set<AMP22MSTMessage *, cmp> message_set;
    std::map<int, FragmentInformation> fragment_informations;
    std::map<std::tuple<int, int, int>, int> pos_of_relay_on_directed_paths;
    std::set<std::tuple<int, int, int>> intra_fragment_directed_edges;
    std::set<std::tuple<int, int, int>> inter_fragment_directed_edges;
    std::multiset<int> awake_rounds_in_phase_set;
    bool on_w_2_path = false;

    virtual void reset_phase() override;
    virtual int get_round_stretch() const override;
    void print_message_set() const;

    std::multiset<int> calculate_awake_rounds_in_phase() const;
    std::set<int> calculate_awake_rounds_in_phase_for_directed_edge(
        const std::tuple<int, int, int> &directed_edge, int pos) const;
    std::set<int>
    calculate_awake_rounds_in_phase_for_directed_edge_same_fragment(
        const std::tuple<int, int, int> &directed_edge, int pos,
        bool stage_for_tail_fragment_only = false) const;
    std::set<int>
    calculate_awake_rounds_in_phase_for_directed_edge_child_to_parent(
        int level_child, int level_parent, int w, int pos,
        bool stage_for_tail_fragment_only = false) const; // for Upcast
    std::set<int>
    calculate_awake_rounds_in_phase_for_directed_edge_parent_to_child(
        int level_parent, int level_child, int w, int pos,
        bool stage_for_tail_fragment_only = false) const; // for Broadcast
    std::set<int>
    calculate_awake_rounds_in_phase_for_directed_edge_different_fragment(
        const std::tuple<int, int, int> &directed_edge, int pos) const;
    std::set<int>
    calculate_awake_rounds_in_phase_for_directed_edge_inter_fragment_2(
        const std::tuple<int, int, int> &directed_edge, int pos) const;

    void remove_head_fragment_awake_rounds_in_stage_for_tail_fragment();

    static int get_redundant_round(int w, int pos);

  public:
    ThreeHopAMP22MSTAlg(Node *node, int starting_round = 1);
    virtual void init(Node *node, int starting_round = 1) override;
    virtual int
    find_next_relay_node_id_on_path_to(int real_receiver_id) override;
    virtual void process_round() override;
    virtual void send_new_message(cMessage *msg,
                                  double delay = DELAY_SENT) override;
    virtual void listen_new_message(cMessage *msg) override;
    virtual void clear_message_queue() override;
    virtual bool is_awake() override;
    virtual ~ThreeHopAMP22MSTAlg();
};

#endif // SCDS_ALGORITHMS_CDS_LAM23_CDS_CONGEST_THREE_HOP_AMP22_MST_H_