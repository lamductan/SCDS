#ifndef SCDS_ALGORITHMS_WAF02_CDS_WAF02_TWO_HOP_MIS_H_
#define SCDS_ALGORITHMS_WAF02_CDS_WAF02_TWO_HOP_MIS_H_

#include "algorithms/cds/WAF02_cds/WAF02_TwoHopMISPart2.h"

class WAF02TwoHopMISStage : public BaseAlgStage
{
  protected:
    static constexpr int get_max_value() { return TWO_HOP_MIS_STAGE; }

  public:
    static const int BFS_STAGE = BaseAlgStage::get_max_value() + 1;
    static const int TWO_HOP_MIS_STAGE = BaseAlgStage::get_max_value() + 2;
};

class WAF02TwoHopMISAlg : public IAlgNode
{
  public:
    int parent_id = INT_MAX;
    int level = INT_MAX;
    std::set<int> children;
    std::pair<int, int> rank;

    std::set<int> mis_neighbors;

    int bfs_starting_round;
    BFSAlg *bfs_alg = nullptr;
    WAF02TwoHopMISPart2Alg *WAF02_TwoHopMIS_part2_alg = nullptr;
    int WAF02_TwoHopMIS_stage = WAF02TwoHopMISStage::INITIAL_STAGE;

  public:
    virtual void set_alg_type() override;
    WAF02TwoHopMISAlg();
    WAF02TwoHopMISAlg(Node *node, int starting_round = 1);
    virtual void handle_message(cMessage *msg) override;
    virtual void stage_transition() override;
    virtual bool is_awake() override;
    virtual bool is_decided() override;
    virtual bool is_selected() override;
    virtual ~WAF02TwoHopMISAlg();
};

#endif // SCDS_ALGORITHMS_WAF02_CDS_WAF02_TWO_HOP_MIS_H_