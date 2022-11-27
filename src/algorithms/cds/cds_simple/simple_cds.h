#ifndef SCDS_ALGORITHMS_CDS_SIMPLE_CDS_H_
#define SCDS_ALGORITHMS_CDS_SIMPLE_CDS_H_

#include "algorithms/cds/cds_simple/simple_mis_to_cds.h"
#include "algorithms/ialg_node.h"
#include "algorithms/mis/SW08_mis/SW08_MIS.h"

class SimpleCDSStage : public BaseAlgStage
{
  protected:
    static constexpr int get_max_value() { return CDS_STAGE; }

  public:
    static const int MIS_STAGE = BaseAlgStage::get_max_value() + 1;
    static const int CDS_STAGE = BaseAlgStage::get_max_value() + 2;
};

class SimpleCDSAlg : public IAlgNode
{
  public:
    int MIS_stage_starting_round = 0;
    int MIS_stage_max_num_rounds = 0;

    int CDS_stage_starting_round = 0;
    int CDS_stage_max_num_rounds = 0;

    int Simple_CDS_stage = SimpleCDSStage::INITIAL_STAGE;

    SW08MISAlg *SW08_MIS_alg = nullptr;
    IAlgNode *CDS_alg = nullptr;

  public:
    void init_MIS();
    virtual void init_CDS();

  public:
    virtual void set_alg_type() override;
    SimpleCDSAlg(Node *node, int starting_round = 1);
    virtual void handle_message(cMessage *msg) override;
    virtual void stage_transition() override;
    virtual bool is_selected() override;

    virtual ~SimpleCDSAlg();
};

#endif // SCDS_ALGORITHMS_CDS_SIMPLE_CDS_H_