#ifndef SCDS_ALGORITHMS_MIS_GP22_MIS_PART2_H_
#define SCDS_ALGORITHMS_MIS_GP22_MIS_PART2_H_

#include "algorithms/ialg_node.h"


class GP22MISAlg;

enum GP22Part2AlgRoundType {
    GP22Part2_DECLARING_STATUS,
    GP22Part2_DECLARING_MARK
};

class GP22MISPart2Alg : public IAlgNode {
public:
    std::unordered_set<int> all_remained_neighbors;
    bool marked = false;

    int num_iterations = 0;
    int num_phases = 0;
    int num_rounds_per_iteration = 0;
    int C;
    int d = 0;
    int log_d = 0;
    int first_awake_round_in_iteration = -1;
    GP22Part2AlgRoundType GP22Part2_alg_round_type = GP22Part2_DECLARING_STATUS;

    int current_round_id_in_iteration = -1;
    int current_phase_id_in_iteration = -1;
    int n_rounds_of_current_phase = -1;

    void init_alg_params(GP22MISAlg *GP22_MIS_alg);
    int find_first_awake_round_in_iteration();
    virtual cMessage *process_message_queue_declaring_status_round();
    virtual cMessage *process_message_queue_declaring_mark_round();

public:
    virtual void set_alg_type() override;
    GP22MISPart2Alg(Node *node, int starting_round=1);
    virtual bool is_awake() override;
    virtual void stage_transition() override;
    virtual cMessage *process_message_queue() override;
    virtual bool is_selected() override;
};

#endif //SCDS_ALGORITHMS_MIS_GP22_MIS_PART2_H_