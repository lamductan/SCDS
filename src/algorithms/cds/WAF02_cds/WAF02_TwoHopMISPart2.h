#ifndef SCDS_ALGORITHMS_WAF02_CDS_WAF02_TWO_HOP_MIS_PART2_H_
#define SCDS_ALGORITHMS_WAF02_CDS_WAF02_TWO_HOP_MIS_PART2_H_

#include "algorithms/bfs/BFS.h"
#include "algorithms/cds/WAF02_cds/WAF02_TwoHopMIS_message.h"
#include "algorithms/ialg_node.h"

class WAF02TwoHopMISPart2Alg : public IAlgNode
{
  public:
    int parent_id = INT_MAX;
    int level = INT_MAX;
    std::set<int> children;
    std::pair<int, int> rank;

    std::set<int> mis_neighbors;
    std::set<int> completed_children;

    bool sent_black_message = false;
    bool sent_gray_message = false;
    bool sent_mark_completed_message = false;

    int x2;
    int y;
    std::set<std::pair<int, int>> lower_rank_neighbors;

    std::set<std::pair<int, int>> get_lower_rank_neighbors(
        const std::set<std::pair<int, int>> &neighbors_rank) const;

    void root_send_first_black_message();
    void
    process_black_message(WAF02TwoHopMISMessage *WAF02_two_hop_MIS_message);
    void process_gray_message(WAF02TwoHopMISMessage *WAF02_two_hop_MIS_message);
    void process_mark_complete_message(
        WAF02TwoHopMISMessage *WAF02_two_hop_MIS_message);

  public:
    virtual void set_alg_type() override;
    WAF02TwoHopMISPart2Alg();
    void init_from_bfs_alg(BFSAlg *bfs_alg);
    virtual void process_round() override;
    void process_WAF02_two_hop_MIS_message_queue();
    void broadcast_message(WAF02TwoHopMISMessage *WAF02_two_hop_MIS_message);
    void send_complete_message_to_parent_when_avaiable();
    virtual bool is_selected() override;
    virtual bool is_awake() override;
    virtual bool is_decided() override;
    WAF02TwoHopMISMessage *create_WAF02_two_hop_MIS_message();

    void print_state(int log_level = 0);
};

#endif // SCDS_ALGORITHMS_WAF02_CDS_WAF02_TWO_HOP_MIS_PART2_H_