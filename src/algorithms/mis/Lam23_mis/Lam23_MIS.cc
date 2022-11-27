#include "algorithms/mis/Lam23_mis/Lam23_MIS.h"
#include "algorithms/mis/Lam23_mis/Lam23_MIS_message.h"
#include "algorithms/network_alg.h"
#include "messages/synchronized_message.h"
#include "utils/utils.h"

void LamMISAlg::set_alg_type()
{
    EV << "GP22MIS::set_alg_type()\n";
    alg_type = MIS_ALG;
}

LamMISAlg::LamMISAlg(Node *node, int starting_round)
{
    init(node, starting_round);

    n = node->n_nodes;
    log_n = log_2(n);

    // part 1
    part1_starting_round = starting_round;
    // part1_max_num_rounds = 2*log_n;
    Lam_MIS_part1_alg = new LamTwoRSAlg(node, part1_starting_round);
    // Lam_MIS_part1_alg->max_num_rounds = part1_max_num_rounds;
    part1_max_num_rounds = Lam_MIS_part1_alg->max_num_rounds;

    // part 1_2
    part1_2_starting_round = starting_round + part1_max_num_rounds;
    part1_2_max_num_rounds = 2;
    Lam_MIS_part1_2_alg =
        new LamInformingNeighborsAlg(node, part1_2_starting_round);
    Lam_MIS_part1_2_alg->max_num_rounds = part1_2_max_num_rounds;

    // part 3
    // We don't run part 2 in simulation, because to achieve f(5) < log(n), n
    // must > 2^f(5) = 2^600
    part3_starting_round = part1_2_starting_round + part1_2_max_num_rounds;
    // part3_max_num_rounds = 2*log_n;
    Lam_MIS_part3_alg = new LamMISPart3Alg(node, part3_starting_round);
    part3_max_num_rounds = Lam_MIS_part3_alg->max_num_rounds;

    max_num_rounds = part3_starting_round + part3_max_num_rounds;

    EV << "LamMISAlg::part1_starting_round = " << part1_starting_round << ' '
       << Lam_MIS_part1_alg->starting_round << '\n';
    EV << "LamMISAlg::part1_2_starting_round = " << part1_2_starting_round
       << ' ' << Lam_MIS_part1_2_alg->starting_round << '\n';
    EV << "LamMISAlg::part3_starting_round = " << part3_starting_round << ' '
       << Lam_MIS_part3_alg->starting_round << '\n';
    EV << "LamMISAlg::total max_num_rounds = " << max_num_rounds << '\n';
}

void LamMISAlg::handle_message(cMessage *msg)
{
    if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
        SynchronizedMessage *synchronized_message =
            dynamic_cast<SynchronizedMessage *>(msg);
        if (synchronized_message->getSynchronizedMessageType() ==
            SYNCHRONIZED_START_ROUND) {
            current_round_id = synchronized_message->getRoundId();
            stage_transition();
        }
    }
    EV << "current_round_id = " << current_round_id << "\n";
    EV << "LamMISAlg::Lam_MIS_stage = " << Lam_MIS_stage << '\n';

    switch (Lam_MIS_stage) {
    case LamMISStage::PART1: call_handle_message(Lam_MIS_part1_alg, msg); break;
    case LamMISStage::PART1_2:
        call_handle_message(Lam_MIS_part1_2_alg, msg);
        break;
    case LamMISStage::PART3: call_handle_message(Lam_MIS_part3_alg, msg); break;
    default: break;
    }

    n_awake_rounds = Lam_MIS_part1_alg->n_awake_rounds +
                     Lam_MIS_part1_2_alg->n_awake_rounds +
                     Lam_MIS_part3_alg->n_awake_rounds;
    EV << "LamMISAlg::status after round #" << current_round_id << " = "
       << MIS_status << ": is_decided() = " << is_decided() << '\n';
    EV << "LamMISAlg::last_communication_round : " << last_communication_round
       << '\n';
    EV << "LamMISAlg::n_awake_rounds = " << n_awake_rounds << '\n';
    EV << "LamMISAlg::decided_round = " << decided_round << '\n';
}

void LamMISAlg::stage_transition()
{
    if (Lam_MIS_stage == LamMISStage::INITIAL_STAGE) {
        Lam_MIS_stage = LamMISStage::PART1;
    } else if (current_round_id == part1_2_starting_round) {
        Lam_MIS_stage = LamMISStage::PART1_2;
        node->all_neighbors = node->all_neighbors_original;
        Lam_MIS_part1_2_alg->two_rs_cluster_center_id =
            Lam_MIS_part1_alg->two_rs_cluster_center_id;
        if (Lam_MIS_part1_alg->Lam_Two_RS_status == LAM_TWO_RS_CLUSTER_CENTER) {
            Lam_MIS_part1_2_alg->MIS_status = MIS_status = IN_MIS;
        } else if (Lam_MIS_part1_alg->Lam_Two_RS_status == LAM_TWO_RS_1_HOP) {
            Lam_MIS_part1_2_alg->MIS_status = MIS_status = NOT_IN_MIS;
        }
        Lam_MIS_part1_2_alg->color =
            Lam_MIS_part1_alg->two_rs_cluster_center_id;
    } else if (current_round_id == part3_starting_round) {
        Lam_MIS_stage = LamMISStage::PART3;
        Lam_MIS_part3_alg->color = Lam_MIS_part1_2_alg->color;
        Lam_MIS_part3_alg->neighbor_colors =
            Lam_MIS_part1_2_alg->neighbor_colors;
        Lam_MIS_part3_alg->MIS_status = MIS_status;
        EV << "LamMISAlg::Lam_MIS_part3_alg->color = "
           << Lam_MIS_part3_alg->color << ", Lam_MIS_part3_alg->MIS_status = "
           << Lam_MIS_part3_alg->MIS_status << "\n";
    }
    EV << "LamMISAlg::MIS_status = " << MIS_status
       << ", decided_round = " << decided_round << '\n';
}

bool LamMISAlg::is_selected() { return (MIS_status == IN_MIS); }

LamMISAlg::~LamMISAlg()
{
    delete Lam_MIS_part1_alg;
    delete Lam_MIS_part1_2_alg;
    delete Lam_MIS_part3_alg;
}
