#include "utils/utils.h"
#include "messages/synchronized_message.h"
#include "algorithms/mis/Lam23_mis/Lam23_MIS_message.h"
#include "algorithms/mis/Lam23_mis/Lam23_MIS_M1.h"
#include "algorithms/network_alg.h"

void LamMISM1Alg::set_alg_type() { EV << "GP22MIS::set_alg_type()\n"; alg_type = MIS_ALG; }

LamMISM1Alg::LamMISM1Alg(Node *node, int starting_round) {
    init(node, starting_round);
    
    n = node->n_nodes;
    log_n = log_2(n);

    // part 1
    part1_starting_round = starting_round;
    //part1_max_num_rounds = 2*log_n;
    Lam_MIS_part1_alg = new LamTwoRSAlg(node, part1_starting_round);
    //Lam_MIS_part1_alg->max_num_rounds = part1_max_num_rounds;
    part1_max_num_rounds = Lam_MIS_part1_alg->max_num_rounds;
    
    // part 3
    // We don't run part 2 in simulation, because to achieve f(5) < log(n), n must > 2^f(5) = 2^600
    part3_starting_round = part1_starting_round + part1_max_num_rounds;
    //part3_max_num_rounds = 2*log_n;
    Lam_MIS_part3_alg = new SW08MISAlg(node, part3_starting_round);
    part3_max_num_rounds = Lam_MIS_part3_alg->max_num_rounds;
    
    max_num_rounds = part3_starting_round + part3_max_num_rounds;

    EV << "LamMISM1Alg::part1_starting_round = " << part1_starting_round << ' ' << Lam_MIS_part1_alg->starting_round << '\n';
    EV << "LamMISM1Alg::part3_starting_round = " << part3_starting_round << ' ' << Lam_MIS_part3_alg->starting_round << '\n';
    EV << "LamMISM1Alg::total max_num_rounds = " << max_num_rounds << '\n';
}

void LamMISM1Alg::handle_message(cMessage *msg) {
    if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
        SynchronizedMessage *synchronized_message = dynamic_cast<SynchronizedMessage *>(msg);
        if (synchronized_message->getSynchronizedMessageType() == SYNCHRONIZED_START_ROUND) {
            current_round_id = synchronized_message->getRoundId();
            stage_transition();
        }
    }
    EV << "current_round_id = " << current_round_id << "\n";
    EV << "LamMISM1Alg::Lam_MIS_stage = " << Lam_MIS_stage << '\n';
    if (Lam_MIS_stage == LamMISStage::PART1) {
        call_handle_message(Lam_MIS_part1_alg, msg);
    } else if (Lam_MIS_stage == LamMISStage::PART3) {
        call_handle_message(Lam_MIS_part3_alg, msg);
    }

    n_awake_rounds = Lam_MIS_part1_alg->n_awake_rounds 
        + Lam_MIS_part3_alg->n_awake_rounds;
    EV << "LamMISM1Alg::status after round #" << current_round_id << " = " << status << ": is_decided() = " << is_decided() << '\n';
    EV << "LamMISM1Alg::last_communication_round : " << last_communication_round << '\n';
    EV << "LamMISM1Alg::n_awake_rounds = " << n_awake_rounds << '\n';
    EV << "LamMISM1Alg::decided_round = " << decided_round << '\n';
}

void LamMISM1Alg::stage_transition() {
    if (Lam_MIS_stage == LamMISStage::INITIAL_STAGE) {
        Lam_MIS_stage = LamMISStage::PART1;
        
    } else if (current_round_id == part3_starting_round) {
        Lam_MIS_stage = LamMISStage::PART3;
        if (Lam_MIS_part1_alg->Lam_Two_RS_status == LAM_TWO_RS_CLUSTER_CENTER) {
            Lam_MIS_part3_alg->status = status = IN_MIS;
            Lam_MIS_part3_alg->SW08_status = SW08_DOMINATOR;
        } else if (Lam_MIS_part1_alg->Lam_Two_RS_status == LAM_TWO_RS_1_HOP) {
            Lam_MIS_part3_alg->status = status = NOT_IN_MIS;
            Lam_MIS_part3_alg->SW08_status = SW08_DOMINATED;
        }

        EV << "LamMISM1Alg::Lam_MIS_part3_alg->SW08_status = " << Lam_MIS_part3_alg->SW08_status << "\n";
    }
    EV << "LamMISM1Alg::status = " << status << '\n';
}

bool LamMISM1Alg::is_selected() {
    return (status == IN_MIS);
}

LamMISM1Alg::~LamMISM1Alg() {
    delete Lam_MIS_part1_alg;
    delete Lam_MIS_part3_alg;
}