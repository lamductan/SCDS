#ifndef SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_PART2_H_
#define SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_PART2_H_

#include "algorithms/cds/Lam23_cds_local/Lam23_BFS_message.h"
#include "algorithms/cds/Lam23_cds_local/Lam23_BFS_part1.h"
#include "algorithms/ialg_node.h"

enum LamBFSRoundPhaseActionType
{
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_MOE,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_MOE,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_MOE,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1, // send
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_2, // receive
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_HEAD,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_HEAD,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_HEAD,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_HEAD,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1, // send
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_2, // receive
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_TAIL,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_TAIL,
    LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL
};

class LamBFSPart2Stage : public BaseAlgStage
{
  protected:
    static constexpr int get_max_value() { return WAITING_FOR_NEXT_STAGE; }

  public:
    static const int FIND_MOE = BaseAlgStage::get_max_value() + 1;
    static const int MERGE = BaseAlgStage::get_max_value() + 2;
    static const int WAITING_FOR_NEXT_STAGE = BaseAlgStage::get_max_value() + 3;
};

class LamBFSPart2Alg : public LamBFSPart1Alg
{
  public:
    int Lam_bfs_part2_stage = LamBFSPart2Stage::INITIAL_STAGE;
    int log2_d = -1;
    int log2_n = -1;
    int current_stage_starting_round = -1;
    int next_stage_starting_round = -1;
    int n_rounds_in_phase_this_stage = -1;
    int next_phase_starting_round = -1;
    int round_id_in_current_phase = -1;

    bool previous_fragment_status = true;

    std::map<int, LamBFSRoundPhaseActionType> awake_rounds_in_phase;
    bool can_merge = false;
    bool is_endpoint_moe = false;
    std::set<int> tail_fragment_nodes;

    void init_from_Lam_BFS_part1_alg(LamBFSPart1Alg *Lam_BFS_part1_alg);

    LamBFSMessage *process_message_queue_UPCAST_SEND_MOE_round();
    LamBFSMessage *process_message_queue_BROADCAST_SEND_MOE_round();
    LamBFSMessage *
    process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_1_round();
    LamBFSMessage *
    process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_2_round();
    LamBFSMessage *process_message_queue_UPCAST_SEND_HEAD_round();
    LamBFSMessage *process_message_queue_BROADCAST_SEND_HEAD_round();
    LamBFSMessage *
    process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_1_round();
    LamBFSMessage *
    process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_2_round();
    LamBFSMessage *process_message_queue_UPCAST_SEND_TAIL_round();
    LamBFSMessage *process_message_queue_BROADCAST_SEND_TAIL_round();

    LamBFSMessage *process_message_queue_BROADCAST_SEND_MOE_round_root();
    LamBFSMessage *process_message_queue_BROADCAST_SEND_MOE_round_non_root();
    LamBFSMessage *process_message_queue_BROADCAST_SEND_HEAD_round_root();
    LamBFSMessage *process_message_queue_BROADCAST_SEND_HEAD_round_non_root();

    int calc_next_stage_starting_round() const;
    int calc_n_rounds_in_phase_this_stage() const;
    int calc_round_id_in_current_phase() const;
    void reset_stage();
    void reset_phase();
    bool is_fragment_leaf() const;
    bool is_fragment_root() const;
    virtual centralized::BFSTreeStructure
    get_bfs_tree_structure() const override;

  public:
    virtual void set_alg_type() override;
    virtual void init(Node *node, int starting_round) override;
    LamBFSPart2Alg();
    LamBFSPart2Alg(Node *node, int starting_round = 1);
    virtual void stage_transition() override;
    virtual cMessage *process_message_queue() override;
    LamBFSMessage *process_message_queue_1();
    virtual bool is_selected() override;
    virtual bool is_awake() override;
    virtual bool is_decided() override;
    virtual void print_state(int log_level = 0) override;
};

std::string LamBFSRoundPhaseActionType_to_string(
    LamBFSRoundPhaseActionType Lam_bfs_round_phase_action_type);

std::string map_LamBFSRoundPhaseActionType_to_string(
    const std::map<int, LamBFSRoundPhaseActionType> &m, int log_level = 0,
    bool log_level_on_first_line = true);

#endif // SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_PART2_H_