#ifndef SCDS_ALGORITHMS_CDS_GK96_CDS_GK96_CDS_H_
#define SCDS_ALGORITHMS_CDS_GK96_CDS_GK96_CDS_H_

#include "algorithms/cds/cds_simple/simple_cds.h"

class GK96CDSStage : public BaseAlgStage
{
  protected:
    static constexpr int get_max_value() { return CDS_STAGE; }

  public:
    static const int MIS_STAGE = BaseAlgStage::get_max_value() + 1;
    static const int THREE_HOP_PATH_CONSTRUCTION_STAGE =
        BaseAlgStage::get_max_value() + 2;
    static const int CDS_STAGE = BaseAlgStage::get_max_value() + 3;
};

class GK96CDSAlg : public SimpleCDSAlg
{
  public:
    int three_hop_path_construction_stage_starting_round = 0;
    int three_hop_path_construction_stage_max_num_rounds = 0;

    IAlgNode *three_hop_path_construction_alg = nullptr;
    int GK96_CDS_stage = GK96CDSStage::INITIAL_STAGE;

  public:
    GK96CDSAlg();
    GK96CDSAlg(Node *node, int starting_round = 1);
    virtual void init(Node *node, int starting_round = 1) override;
    virtual void init_three_hop_path_construction_alg();
    virtual void init_CDS() override;
    virtual void handle_message(cMessage *msg) override;
    virtual void stage_transition() override;
    virtual void init_three_hop_mst();
    virtual bool is_awake() override;
    virtual bool is_decided() override;
    virtual bool is_selected() override;
    virtual ~GK96CDSAlg();
};

#endif // SCDS_ALGORITHMS_CDS_GK96_CDS_GK96_CDS_H_