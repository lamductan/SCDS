#ifndef SCDS_ALGORITHMS_CDS_SIMPLE_CDS_2_H_
#define SCDS_ALGORITHMS_CDS_SIMPLE_CDS_2_H_

#include "algorithms/cds/cds_simple/simple_mis_to_cds.h"
#include "algorithms/ialg_node.h"
#include "algorithms/mis/Lam23_mis/Lam23_MIS.h"

class SimpleCDS2Stage : public BaseAlgStage
{
  protected:
    static constexpr int get_max_value() { return CDS_STAGE; }

  public:
    static const int MIS_STAGE = BaseAlgStage::get_max_value() + 1;
    static const int CDS_STAGE = BaseAlgStage::get_max_value() + 2;
};

class SimpleCDS2Alg : public IAlgNode
{
  public:
    int MIS_stage_starting_round = 0;
    int MIS_stage_max_num_rounds = 0;

    int CDS_stage_starting_round = 0;
    int CDS_stage_max_num_rounds = 0;

    int Simple_CDS_stage = SimpleCDS2Stage::INITIAL_STAGE;

    LamMISAlg *Lam_MIS_alg = nullptr;
    IAlgNode *CDS_alg = nullptr;

  public:
    void init_MIS();
    virtual void init_CDS();

  public:
    virtual void set_alg_type() override;
    SimpleCDS2Alg();
    SimpleCDS2Alg(Node *node, int starting_round = 1);
    virtual void handle_message(cMessage *msg) override;
    virtual void stage_transition() override;
    virtual bool is_selected() override;

    virtual ~SimpleCDS2Alg();
};

#endif // SCDS_ALGORITHMS_CDS_SIMPLE_CDS_2_H_