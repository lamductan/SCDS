#ifndef SCDS_ALGORITHMS_MIS_LUBY_MIS_H_
#define SCDS_ALGORITHMS_MIS_LUBY_MIS_H_

#include "algorithms/ialg_node.h"

enum LubyAlgRoundType
{
    LUBY_GENERATING_MARK,
    LUBY_PROCESSING_MARK
};

class LubyMISStage : public BaseAlgStage
{
  protected:
    static constexpr int get_max_value() { return MIS_STAGE; }

  public:
    static const int MIS_STAGE = BaseAlgStage::get_max_value() + 1;
};

class LubyMISAlg : public IAlgNode
{
  public:
    bool marked = false;
    int degree;
    LubyAlgRoundType Luby_alg_round_type = LUBY_PROCESSING_MARK;

    cMessage *process_message_queue_for_generate_mark_round();
    cMessage *process_message_queue_for_processing_mark_round();

  public:
    virtual void set_alg_type() override;
    LubyMISAlg(Node *node, int starting_round = 1);
    virtual void stage_transition() override;
    virtual cMessage *process_message_queue() override;
    virtual bool is_selected() override;
};

#endif // SCDS_ALGORITHMS_MIS_LUBY_MIS_H_