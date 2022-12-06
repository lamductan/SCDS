#ifndef SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_H_
#define SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_H_

#include "algorithms/cds/Lam23_cds_local/Lam23_BFS_part2.h"

class LamBFSStage : public BaseAlgStage
{
  protected:
    static constexpr int get_max_value() { return BFS_PART_2; }

  public:
    static const int BFS_PART_1 = BaseAlgStage::get_max_value() + 1;
    static const int BFS_PART_2 = BaseAlgStage::get_max_value() + 2;
};

class LamBFSAlg : public IAlgNode
{
  public:
    int Lam_BFS_part1_starting_round = -1;
    LamBFSPart1Alg *Lam_BFS_part1_alg = nullptr;
    int Lam_BFS_part2_starting_round = -1;
    LamBFSPart2Alg *Lam_BFS_part2_alg = nullptr;
    int Lam_BFS_stage = LamBFSStage::INITIAL_STAGE;

  public:
    virtual void set_alg_type() override;
    LamBFSAlg();
    LamBFSAlg(Node *node, int starting_round = 1);
    virtual void handle_message(cMessage *msg) override;
    virtual void stage_transition() override;
    virtual bool is_awake() override;
    virtual bool is_decided() override;
    virtual bool is_selected() override;
    virtual ~LamBFSAlg();
};

#endif // SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_H_