#ifndef SCDS_ALGORITHMS_MIS_LAM23_PART_1_TWO_RS_H_
#define SCDS_ALGORITHMS_MIS_LAM23_PART_1_TWO_RS_H_

#include "algorithms/ialg_node.h"
#include "algorithms/mis/Lam23_mis/Lam23_Part1_Two_RS_BGKO22.h"
#include "algorithms/mis/Lam23_mis/Lam23_InformingNeighbors.h"
#include "algorithms/mis/SW08_mis/SW08_MIS.h"


class LamTwoRSStage : public BaseAlgStage {
protected:
    static constexpr int get_max_value() { return PART2; }
public:
    static const int PART1 = BaseAlgStage::get_max_value() + 1; // 2-RulingSet-BGKO22
    static const int PART1_2 = BaseAlgStage::get_max_value() + 2; // Informing 2-RulingSet-BGKO22 result
    static const int PART2 = BaseAlgStage::get_max_value() + 3; // MIS-SW08
};


class LamTwoRSAlg : public IAlgNode {
public:
    int n;
    int log_n;

    int part1_starting_round = 0;
    int part1_max_num_rounds = 0;
    
    int part1_2_starting_round = 0;
    int part1_2_max_num_rounds = 0;

    int part2_starting_round = 0;
    int part2_max_num_rounds = 0;

    int Lam_Two_RS_stage = LamTwoRSStage::INITIAL_STAGE;
    LamTwoRSNodeStatus Lam_Two_RS_status = LAM_TWO_RS_UNDECIDED;

    BGKO22TwoRSAlg *Lam_Two_RS_part1_alg = nullptr;
    LamInformingNeighborsAlg *Lam_Two_RS_part1_2_alg = nullptr;
    SW08MISAlg *Lam_Two_RS_part2_alg = nullptr;

    int two_rs_cluster_center_id = -1;

public:
    virtual void set_alg_type() override;
    LamTwoRSAlg(Node *node, int starting_round=1);
    virtual void handle_message(cMessage *msg) override;
    virtual void stage_transition() override;
    virtual bool is_selected() override;
    virtual bool is_decided() override;
    virtual void call_handle_message(IAlgNode *alg, cMessage *msg) override;

    virtual ~LamTwoRSAlg();
};

#endif //SCDS_ALGORITHMS_MIS_LAM23_PART_1_TWO_RS_H_