#include "algorithms/cds/Lam23_cds_local/Lam23_BFS.h"
#include "messages/synchronized_message.h"

void LamBFSAlg::set_alg_type()
{
    EV << "LamBFSAlg::set_alg_type()\n";
    alg_type = BFS_ALG;
}

LamBFSAlg::LamBFSAlg(Node *node, int starting_round)
{
    init(node, starting_round);
    Lam_BFS_part1_starting_round = starting_round;
    Lam_BFS_part1_alg = new LamBFSPart1Alg(node, Lam_BFS_part1_starting_round);
    Lam_BFS_part2_starting_round =
        Lam_BFS_part1_starting_round + Lam_BFS_part1_alg->max_num_rounds;
    Lam_BFS_part2_alg = new LamBFSPart2Alg();
    max_num_rounds = 33; // TODO: for DEBUG purpose only
}

void LamBFSAlg::handle_message(cMessage *msg)
{
    handle_synchronized_message(msg);
    EV << "current_round_id = " << current_round_id << "\n";
    EV << "Lam_BFS_stage = " << Lam_BFS_stage << "\n";
    switch (Lam_BFS_stage) {
    case LamBFSStage::BFS_PART_1:
        call_handle_message(Lam_BFS_part1_alg, msg);
        break;
    case LamBFSStage::BFS_PART_2:
        call_handle_message(Lam_BFS_part2_alg, msg);
        break;
    default:
        break;
    }

    n_awake_rounds =
        Lam_BFS_part1_alg->n_awake_rounds + Lam_BFS_part2_alg->n_awake_rounds;
    EV << "last_communication_round : " << last_communication_round << '\n';
}

void LamBFSAlg::stage_transition()
{
    if (Lam_BFS_stage == LamBFSStage::INITIAL_STAGE) {
        Lam_BFS_stage = LamBFSStage::BFS_PART_1;
    } else if (current_round_id == Lam_BFS_part2_starting_round) {
        Lam_BFS_stage = LamBFSStage::BFS_PART_2;
        Lam_BFS_part2_alg->init_from_Lam_BFS_part1_alg(Lam_BFS_part1_alg);
    }
}

bool LamBFSAlg::is_awake()
{
    switch (Lam_BFS_stage) {
    case LamBFSStage::BFS_PART_1:
        return Lam_BFS_part1_alg->is_awake();
    case LamBFSStage::BFS_PART_2:
        return Lam_BFS_part2_alg->is_awake();
    default:
        return true;
    }
}

bool LamBFSAlg::is_decided()
{
    if (Lam_BFS_part2_alg == nullptr) return false;
    return Lam_BFS_part2_alg->is_decided();
}

bool LamBFSAlg::is_selected() { return true; }

LamBFSAlg::~LamBFSAlg()
{
    delete Lam_BFS_part1_alg;
    delete Lam_BFS_part2_alg;
}