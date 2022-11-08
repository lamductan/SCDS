#ifndef SCDS_ALGORITHMS_MIS_LUBY_MIS_H_
#define SCDS_ALGORITHMS_MIS_LUBY_MIS_H_

#include "algorithms/ialg_node.h"

class LubyMISStage : public BaseAlgStage {
protected:
    static constexpr int get_max_value() { return MIS_STAGE; }
public:
    static const int MIS_STAGE = BaseAlgStage::get_max_value() + 1;
};


class LubyMISAlg : public IAlgNode {
protected:
    bool marked = false;
    NodeStatus status = UNDECIDED;
    int degree;

    NodeStatus previous_status = UNDECIDED;

    cMessage *create_Luby_mis_message();
    bool is_marked_round();

public:
    virtual void set_alg_type() override;
    LubyMISAlg(Node *node);
    virtual void stage_transition() override;
    virtual cMessage *process_message_queue() override;
    virtual bool is_selected() override;
};

#endif //SCDS_ALGORITHMS_MIS_LUBY_MIS_H_