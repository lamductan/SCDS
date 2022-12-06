#ifndef SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_PART_1_H_
#define SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_PART_1_H_

#include "algorithms/cds/Lam23_cds_local/Lam23_BFS_message.h"
#include "algorithms/ialg_node.h"

class LamBFSPart1Alg : public IAlgNode
{
  public:
    int d = -1;
    LamBFSFragmentInformation fragment_information;
    LamBFSFragmentInformation new_fragment_information;
    std::tuple<int, int, int> fragment_moe = INF_EDGE_ID;
    int phase_id = -1;

    std::set<int> different_fragment_neighbors;
    std::map<int, std::tuple<int, int, int>> edges_map;
    std::set<std::tuple<int, int, int>> basic_edges_set;
    centralized::BFSTreeStructure bfs_tree_structure;
    LamBFSCoinValue coin_value = LAM_BFS_COIN_VALUE_UNDEFINED;
    std::tuple<int, int, int> node_moe = INF_EDGE_ID;
    bool updated_fragment_information_in_current_phase = false;

    cMessage *first_round_send_coin_value_message();
    cMessage *second_round_send_bfs_structure_message();
    cMessage *third_round_receive_bfs_structure_message();

    LamBFSMessage *create_Lam_bfs_message();

    bool fragment_root_compute_new_bfs_tree_structure();

    void update_fragment_information();
    void
    update_new_fragment_information_from_msg(LamBFSMessage *Lam_bfs_message);
    void print_message_queue();

    LamBFSCoinValue toss_coin() const;

  public:
    virtual void set_alg_type() override;
    virtual void init(Node *node, int starting_round) override;
    LamBFSPart1Alg();
    LamBFSPart1Alg(Node *node, int starting_round = 1);
    virtual cMessage *process_message_queue() override;
    virtual bool is_selected() override;
    virtual bool is_awake() override;
    virtual bool is_decided() override;
    virtual void print_state(int log_level = 0) override;
};

#endif // SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_PART_1_H_