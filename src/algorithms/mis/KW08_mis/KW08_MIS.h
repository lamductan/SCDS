#ifndef SCDS_ALGORITHMS_MIS_KW08_MIS_H_
#define SCDS_ALGORITHMS_MIS_KW08_MIS_H_

#include <unordered_map>
#include <unordered_set>
#include "algorithms/ialg_node.h"

enum KW08NodeStatus {
    KW08_COMPETITOR,
    KW08_RULER,
    KW08_RULED,
    KW08_DOMINATOR,
    KW08_DOMINATED
};

enum KW08AlgRoundType {
    KW08_EXCHANGING_R,
    KW08_EXCHANGING_STATE_1,
    KW08_EXCHANGING_STATE_2
};


class KW08MISStage : public BaseAlgStage {
protected:
    static constexpr int get_max_value() { return MIS_STAGE; }
public:
    static const int MIS_STAGE = BaseAlgStage::get_max_value() + 1;
};


class KW08MISAlg : public IAlgNode {
public:
    static const int N_ROUNDS_PER_COMPETITION = 3;
    static const int N_PHASES_PER_STAGE = 5;
    static const int N_STAGES = 2;
    
    void init_alg_variables();
    bool reset_stage_if_needed();
    bool reset_phase_if_needed();

    int n_competitions_per_phase;
    int n_rounds_per_phase;
    int n_rounds_per_stage;

    int r;
    std::unordered_set<int> undecided_neighbors;
    std::unordered_map<int, KW08NodeStatus> neighbors_status;
    std::unordered_map<int, int> neighbors_r;
    KW08NodeStatus KW08_previous_status = KW08_COMPETITOR;

    KW08AlgRoundType KW08_alg_round_type = KW08_EXCHANGING_STATE_2;

    bool is_decided_status(KW08NodeStatus node_status);
    int find_smallest_r();
    bool is_new_stage();
    bool is_new_phase();
    virtual void record_decided_round() override;

    cMessage *process_message_queue_for_exchange_r_round();
    cMessage *process_message_queue_for_exchange_state_1_round();
    cMessage *process_message_queue_for_exchange_state_2_round();

    void set_need_to_send_to_competitor_neighbors();
    void set_need_to_send_to_undecided_neighbors();

public:
    KW08NodeStatus KW08_status = KW08_COMPETITOR;
    virtual void set_alg_type() override;
    KW08MISAlg(Node *node, int starting_round=1);
    virtual void stage_transition() override;
    virtual cMessage *process_message_queue() override;
    virtual bool is_selected() override;
    virtual bool is_decided() override;
    virtual void update_previous_status() override;
};

#endif //SCDS_ALGORITHMS_MIS_KW08_MIS_H_