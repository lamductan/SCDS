#ifndef SCDS_ALGORITHMS_WAF02_CDS_WAF02_CDS_H_
#define SCDS_ALGORITHMS_WAF02_CDS_WAF02_CDS_H_

#include "algorithms/cds/WAF02_cds/WAF02_CDS_part3.h"
#include "algorithms/cds/WAF02_cds/WAF02_TwoHopMIS.h"

class WAF02CDSStage : public BaseAlgStage
{
  protected:
    static constexpr int get_max_value() { return CDS_STAGE; }

  public:
    static const int TWO_HOP_MIS_STAGE = BaseAlgStage::get_max_value() + 1;
    static const int CDS_STAGE = BaseAlgStage::get_max_value() + 2;
};

class WAF02CDSAlg : public IAlgNode
{
  public:
    int WAF02_TwoHopMIS_starting_round = -1;
    WAF02TwoHopMISAlg *WAF02_TwoHopMIS_alg = nullptr;
    WAF02CDSPart3Alg *WAF02_CDS_part3_alg = nullptr;
    int WAF02_CDS_stage = WAF02CDSStage::INITIAL_STAGE;

  public:
    virtual void set_alg_type() override;
    WAF02CDSAlg();
    WAF02CDSAlg(Node *node, int starting_round = 1);
    virtual void handle_message(cMessage *msg) override;
    virtual void stage_transition() override;
    virtual bool is_awake() override;
    virtual bool is_decided() override;
    virtual bool is_selected() override;
    virtual ~WAF02CDSAlg();
};

#endif // SCDS_ALGORITHMS_WAF02_CDS_WAF02_CDS_H_