#include "utils/utils.h"
#include "messages/synchronized_message.h"
#include "algorithms/mis/Lam23_mis/Lam23_MIS_message.h"
#include "algorithms/mis/Lam23_mis/Lam23_Part1_Two_RS.h"

void LamTwoRSAlg::set_alg_type() { EV << "LamTwoRSAlg::set_alg_type()\n"; alg_type = TWO_RS_ALG; }

LamTwoRSAlg::LamTwoRSAlg(Node *node, int starting_round) {
    init(node, starting_round);
    
    n = node->n_nodes;
    log_n = log_2(n);

    // part 1
    part1_starting_round = starting_round;
    part1_max_num_rounds = 3*std::min(2, log_2(log_star(n)));
    Lam_Two_RS_part1_alg = new BGKO22TwoRSAlg(node, part1_starting_round);
    Lam_Two_RS_part1_alg->max_num_rounds = part1_max_num_rounds;
    
    // part 1_2
    part1_2_starting_round = starting_round + part1_max_num_rounds;
    part1_2_max_num_rounds = 2;
    Lam_Two_RS_part1_2_alg = new LamInformingNeighborsAlg(node, part1_2_starting_round);
    Lam_Two_RS_part1_2_alg->max_num_rounds = part1_2_max_num_rounds;
    
    // part 2
    part2_starting_round = part1_2_starting_round + part1_2_max_num_rounds;
    Lam_Two_RS_part2_alg = new SW08MISAlg(node, part2_starting_round);
    
    part2_max_num_rounds = 20;
    Lam_Two_RS_part2_alg->max_num_rounds = part2_max_num_rounds;
    //part2_max_num_rounds = Lam_Two_RS_part2_alg->max_num_rounds; // either use it or the two above lines
    
    max_num_rounds = part2_starting_round + part2_max_num_rounds;

    EV << "LamTwoRSAlg::part1_starting_round = " << part1_starting_round << ' ' << Lam_Two_RS_part1_alg->starting_round << '\n';
    EV << "LamTwoRSAlg::part1_2_starting_round = " << part1_2_starting_round << ' ' << Lam_Two_RS_part1_2_alg->starting_round << '\n';
    EV << "LamTwoRSAlg::part2_starting_round = " << part2_starting_round << ' ' << Lam_Two_RS_part2_alg->starting_round << '\n';
    EV << "LamTwoRSAlg::total max_num_rounds = " << max_num_rounds << '\n';
}

void LamTwoRSAlg::handle_message(cMessage *msg) {
    EV << "Lam_Two_RS_stage = " << Lam_Two_RS_stage << '\n';
    if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
        SynchronizedMessage *synchronized_message = dynamic_cast<SynchronizedMessage *>(msg);
        if (synchronized_message->getSynchronizedMessageType() == SYNCHRONIZED_START_ROUND) {
            current_round_id = synchronized_message->getRoundId();
            stage_transition();
        }
    }
    EV << "current_round_id = " << current_round_id << "\n";
    switch (Lam_Two_RS_stage) {
    case LamTwoRSStage::PART1:
        call_handle_message(Lam_Two_RS_part1_alg, msg);
        break;
    case LamTwoRSStage::PART1_2:
        call_handle_message(Lam_Two_RS_part1_2_alg, msg);
        break;
    case LamTwoRSStage::PART2:
        call_handle_message(Lam_Two_RS_part2_alg, msg);
        break;
    case LamTwoRSStage::END_STAGE:
        delete msg;
        break;
    default:
        break;
    }

    if (Lam_Two_RS_stage == LamTwoRSStage::END_STAGE) return;
    n_awake_rounds = Lam_Two_RS_part1_alg->n_awake_rounds 
        + Lam_Two_RS_part1_2_alg->n_awake_rounds
        + Lam_Two_RS_part2_alg->n_awake_rounds;
    EV << "LamTwoRSAlg::status after round #" << current_round_id << " = " << Lam_Two_RS_status << ": is_decided() = " << is_decided() << '\n';
    EV << "LamTwoRSAlg::last_communication_round : " << last_communication_round << '\n';
    EV << "LamTwoRSAlg::n_awake_rounds = " << n_awake_rounds << '\n';
    EV << "LamTwoRSAlg::decided_round = " << decided_round << '\n';
}

void LamTwoRSAlg::stage_transition() {
    EV << "LamTwoRSAlg::status = " << MIS_status << '\n';
    if (Lam_Two_RS_stage == LamTwoRSStage::INITIAL_STAGE) {
        Lam_Two_RS_stage = LamTwoRSStage::PART1;
        
    } else if (current_round_id == part1_2_starting_round) {
        Lam_Two_RS_stage = LamTwoRSStage::PART1_2;
        two_rs_cluster_center_id = Lam_Two_RS_part1_alg->two_rs_cluster_center_id;
        Lam_Two_RS_part1_2_alg->two_rs_cluster_center_id = two_rs_cluster_center_id;
        Lam_Two_RS_part1_2_alg->Lam_Two_RS_status = Lam_Two_RS_part1_alg->BGKO22_Two_RS_status;
        Lam_Two_RS_part1_2_alg->all_remained_neighbors = Lam_Two_RS_part1_alg->all_remained_neighbors;

    } else if (current_round_id == part2_starting_round) {
        if (Lam_Two_RS_status != LAM_TWO_RS_UNDECIDED) Lam_Two_RS_stage = LamTwoRSStage::END_STAGE;
        else {
            Lam_Two_RS_stage = LamTwoRSStage::PART2;
            node->all_neighbors = std::vector<int>(
                Lam_Two_RS_part1_2_alg->all_remained_neighbors.begin(),
                Lam_Two_RS_part1_2_alg->all_remained_neighbors.end()
            );
            std::sort(node->all_neighbors.begin(), node->all_neighbors.end());
            EV << "LamTwoRSAlg::node->all_neighbors = [";
            for(int x : node->all_neighbors) EV << x << ',';
            EV << "]\n";
        }
    }
}

void LamTwoRSAlg::call_handle_message(IAlgNode *alg, cMessage *msg) {
    IAlgNode::call_handle_message(alg, msg);
    switch (Lam_Two_RS_stage) {
    case LamTwoRSStage::PART1: {
        BGKO22TwoRSAlg *alg_1 = dynamic_cast<BGKO22TwoRSAlg *>(alg);
        two_rs_cluster_center_id = alg_1->two_rs_cluster_center_id;
        Lam_Two_RS_status = alg_1->BGKO22_Two_RS_status;
        break;
    }
    case LamTwoRSStage::PART1_2: {
        LamInformingNeighborsAlg *alg_1_2 = dynamic_cast<LamInformingNeighborsAlg *>(alg);
        Lam_Two_RS_status = alg_1_2->Lam_Two_RS_status;
        break;
    }
    case LamTwoRSStage::PART2: {
        SW08MISAlg *alg_2 = dynamic_cast<SW08MISAlg *>(alg);
        if (alg_2->SW08_status == SW08_DOMINATOR) {
            Lam_Two_RS_status = LAM_TWO_RS_CLUSTER_CENTER;
            two_rs_cluster_center_id = alg_2->dominator;
        } else if (alg_2->SW08_status == SW08_DOMINATED) {
            Lam_Two_RS_status = LAM_TWO_RS_1_HOP;
            two_rs_cluster_center_id = alg_2->dominator;
        }
        break;
    }
    default:
        break;
    }

    if (Lam_Two_RS_status == LAM_TWO_RS_CLUSTER_CENTER) {
        MIS_status = IN_MIS;
        if (previous_MIS_status == UNDECIDED) {
            decided_round = current_round_id;
            EV << "LamTwoRSAlg::call_handle_message() --- decided_round = " << decided_round << '\n';
        }
    } else if (Lam_Two_RS_status == LAM_TWO_RS_1_HOP) {
        MIS_status = NOT_IN_MIS;
        if (previous_MIS_status == UNDECIDED) {
            decided_round = current_round_id;
            EV << "LamTwoRSAlg::call_handle_message() --- decided_round = " << decided_round << '\n';
        }
    }
    EV << "LamTwoRSAlg::call_handle_message: " << "Lam_Two_RS_status = " << Lam_Two_RS_status
       << ", two_rs_cluster_center_id = " << two_rs_cluster_center_id << '\n';
}

bool LamTwoRSAlg::is_selected() {
    return (Lam_Two_RS_status == LAM_TWO_RS_CLUSTER_CENTER);
}

bool LamTwoRSAlg::is_decided() {
    return (Lam_Two_RS_status != LAM_TWO_RS_UNDECIDED);
}

LamTwoRSAlg::~LamTwoRSAlg() {
    delete Lam_Two_RS_part1_alg;
    delete Lam_Two_RS_part1_2_alg;
    delete Lam_Two_RS_part2_alg;
}