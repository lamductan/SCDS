#ifndef SCDS_ALGORITHMS_MIS_GP22_MIS_PART1_2_H_
#define SCDS_ALGORITHMS_MIS_GP22_MIS_PART1_2_H_

#include "algorithms/ialg_node.h"


class GP22MISPart1_2_Alg : public IAlgNode {
public:
    std::unordered_set<int> all_remained_neighbors;
    bool marked = false;

public:
    virtual void set_alg_type() override;
    GP22MISPart1_2_Alg(Node *node, int starting_round=1);
    virtual cMessage *process_message_queue() override;
    virtual bool is_selected() override;
    virtual bool is_awake() override;
};

#endif //SCDS_ALGORITHMS_MIS_GP22_MIS_PART1_2_H_