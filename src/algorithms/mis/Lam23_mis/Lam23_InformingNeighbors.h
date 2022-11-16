#ifndef SCDS_ALGORITHMS_MIS_LAM23_MIS_LAM23_INFORMING_NEIGHBORS_H_
#define SCDS_ALGORITHMS_MIS_LAM23_MIS_LAM23_INFORMING_NEIGHBORS_H_

#include "algorithms/ialg_node.h"
#include "algorithms/mis/Lam23_mis/Lam23_Part1_Two_RS_BGKO22.h"


class LamInformingNeighborsAlg : public IAlgNode {
public:
    int two_rs_cluster_center_id = -1;
    int color = -1;
    LamTwoRSNodeStatus Lam_Two_RS_status = LAM_TWO_RS_UNDECIDED;
    std::set<int> all_remained_neighbors;
    std::vector<int> neighbor_clusters;
    std::set<std::pair<int,int>> neighbor_colors; //TODO: init color in LamMISAlg stage_transition().

public:
    virtual void set_alg_type() override;
    LamInformingNeighborsAlg(Node *node, int starting_round=1);
    virtual cMessage *process_message_queue() override;
    virtual bool is_selected() override;
    virtual bool is_awake() override;
};

#endif //SCDS_ALGORITHMS_MIS_LAM23_MIS_LAM23_INFORMING_NEIGHBORS_H_