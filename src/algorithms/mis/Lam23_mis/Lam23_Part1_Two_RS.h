#ifndef SCDS_ALGORITHMS_MIS_LAM23_MIS_LAM23_PART1_TWO_RS_H_
#define SCDS_ALGORITHMS_MIS_LAM23_MIS_LAM23_PART1_TWO_RS_H_

#include "algorithms/ialg_node.h"

enum LamTwoRSNodeStatus {
    LAM_TWO_RS_UNDECIDED,
    LAM_TWO_RS_CLUSTER_CENTER,
    LAM_TWO_RS_1_HOP,
    LAM_TWO_RS_2_HOP
};

enum LamTwoRSAlgRoundType {
    LAM_TWO_RS_GENERATING_MARK,
    LAM_TWO_RS_INFORMING_STATUS_1,
    LAM_TWO_RS_INFORMING_STATUS_2,
};

class LamTwoRSStage : public BaseAlgStage {
protected:
    static constexpr int get_max_value() { return LAM_TWO_RS_STAGE; }
public:
    static const int LAM_TWO_RS_STAGE = BaseAlgStage::get_max_value() + 1;
};


class LamTwoRSAlg : public IAlgNode {
public:
    bool marked = false;
    int degree;
    LamTwoRSNodeStatus Lam_Two_RS_status = LAM_TWO_RS_UNDECIDED;
    LamTwoRSNodeStatus Lam_Two_RS_previous_status = LAM_TWO_RS_UNDECIDED;
    LamTwoRSAlgRoundType Lam_Two_RS_alg_round_type = LAM_TWO_RS_INFORMING_STATUS_2;

    cMessage *process_message_queue_for_generate_mark_round();
    cMessage *process_message_queue_for_informing_status_1_round();
    cMessage *process_message_queue_for_informing_status_2_round();

public:
    virtual void set_alg_type() override;
    LamTwoRSAlg(Node *node, int starting_round=1);
    virtual void stage_transition() override;
    virtual cMessage *process_message_queue() override;
    virtual bool is_selected() override;
    virtual bool is_decided() override;
    virtual void update_previous_status() override;
    virtual void record_decided_round() override;
};

#endif //SCDS_ALGORITHMS_MIS_LAM23_MIS_LAM23_PART1_TWO_RS_H_