#ifndef SCDS_ALGORITHMS_MIS_GP22_MIS_H_
#define SCDS_ALGORITHMS_MIS_GP22_MIS_H_

#include "algorithms/ialg_node.h"
#include "algorithms/mis/GP22_mis/GP22_MIS_part1.h"
#include "algorithms/mis/GP22_mis/GP22_MIS_part1_2.h"
#include "algorithms/mis/GP22_mis/GP22_MIS_part2.h"
#include "algorithms/mis/Luby_mis/Luby_MIS.h"


class GP22MISStage : public BaseAlgStage {
protected:
    static constexpr int get_max_value() { return PART3; }
public:
    static const int PART1 = BaseAlgStage::get_max_value() + 1;
    static const int PART1_2 = BaseAlgStage::get_max_value() + 2;
    static const int PART2 = BaseAlgStage::get_max_value() + 3;
    static const int PART3 = BaseAlgStage::get_max_value() + 4;
};


class GP22MISAlg : public IAlgNode {
public:
    int n;

    int part1_starting_round = 0;
    int part1_max_num_rounds = 0;
    
    int part1_2_starting_round = 0;
    int part1_2_max_num_rounds = 0;

    int part2_starting_round = 0;
    int part2_num_iterations = 0;
    int part2_num_phases = 0;
    int part2_num_rounds_per_iteration = 0;
    int part2_max_num_rounds = 0;
    
    int part3_starting_round = 0;
    int part3_max_num_rounds = 0;
    
    int log_n;
    int loglog_n;
    int d;
    int log_d;
    int threshold;
    static const int C = 2;

    int GP22_MIS_stage = GP22MISStage::INITIAL_STAGE;

    GP22MISPart1Alg *GP22_MIS_part1_alg = nullptr;
    GP22MISPart1_2_Alg *GP22_MIS_part1_2_alg = nullptr;
    GP22MISPart2Alg *GP22_MIS_part2_alg = nullptr;
    LubyMISAlg *GP22_MIS_part3_alg = nullptr;

    std::unordered_set<int> all_remained_neighbors;

public:
    virtual void set_alg_type() override;
    GP22MISAlg(Node *node, int starting_round=1);
    virtual void handle_message(cMessage *msg) override;
    virtual void stage_transition() override;
    virtual bool is_selected() override;

    virtual ~GP22MISAlg();
};

#endif //SCDS_ALGORITHMS_MIS_GP22_MIS_H_