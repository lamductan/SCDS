#ifndef SCDS_ALGORITHMS_MIS_LAM23_MIS_M1_H_
#define SCDS_ALGORITHMS_MIS_LAM23_MIS_M1_H_

#include "algorithms/mis/Lam23_mis/Lam23_MIS.h"
#include "algorithms/mis/SW08_mis/SW08_MIS.h"


class LamMISM1Alg : public IAlgNode {
public:
    int n;
    int log_n;

    int part1_starting_round = 0;
    int part1_max_num_rounds = 0;
    
    int part3_starting_round = 0;
    int part3_max_num_rounds = 0;

    int Lam_MIS_stage = LamMISStage::INITIAL_STAGE;

    LamTwoRSAlg *Lam_MIS_part1_alg = nullptr;
    SW08MISAlg *Lam_MIS_part3_alg = nullptr;

public:
    virtual void set_alg_type() override;
    LamMISM1Alg(Node *node, int starting_round=1);
    virtual void handle_message(cMessage *msg) override;
    virtual void stage_transition() override;
    virtual bool is_selected() override;

    virtual ~LamMISM1Alg();
};

#endif //SCDS_ALGORITHMS_MIS_LAM23_MIS_M1_H_