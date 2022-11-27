#include "algorithms/mis/GP22_mis/GP22_MIS.h"
#include "algorithms/mis/GP22_mis/GP22_MIS_message.h"
#include "algorithms/network_alg.h"
#include "messages/synchronized_message.h"
#include "utils/utils.h"

void GP22MISAlg::set_alg_type()
{
    EV << "GP22MIS::set_alg_type()\n";
    alg_type = MIS_ALG;
}

GP22MISAlg::GP22MISAlg(Node *node, int starting_round, double threshold_ratio)
{
    init(node, starting_round);
    all_remained_neighbors =
        std::set<int>(node->all_neighbors.begin(), node->all_neighbors.end());

    n = node->n_nodes;

    log_n = log_2(n);
    loglog_n = log_2(log_n);
    d = 2 * log_n * log_n;
    log_d = log_2(d);
    threshold = ceil(threshold_ratio * n / log_n);
    // threshold = ceil(0.9*n);
    EV << "threshold = " << threshold << "\n";

    // part 1
    part1_starting_round = starting_round;
    part1_max_num_rounds = log_n;
    GP22_MIS_part1_alg = new GP22MISPart1Alg(node, part1_starting_round);
    GP22_MIS_part1_alg->max_num_rounds = part1_max_num_rounds;

    // part 1_2
    part1_2_starting_round = starting_round + part1_max_num_rounds;
    part1_2_max_num_rounds = 3;
    GP22_MIS_part1_2_alg = new GP22MISPart1_2_Alg(node, part1_2_starting_round);
    GP22_MIS_part1_2_alg->max_num_rounds = part1_2_max_num_rounds;

    // part 2
    // We don't run part 2 in simulation, because to achieve log^4(logn) <
    // log(n), n must > 10^(10^4)!
    part2_starting_round = -1;
    part2_num_iterations = loglog_n;
    part2_num_phases = log_d;
    part2_num_rounds_per_iteration =
        C * ((log_d - 1) * (log_d) * (2 * log_d - 1) / 6) + 1;
    part2_max_num_rounds =
        part2_num_iterations * part2_num_rounds_per_iteration;
    GP22_MIS_part2_alg = new GP22MISPart2Alg(node, part2_starting_round);

    // part 3
    // We don't run part 2 in simulation, because to achieve log^4(logn) <
    // log(n), n must > 10^(10^4)!
    part3_starting_round = part1_2_starting_round + part1_2_max_num_rounds;
    part3_max_num_rounds = 3 * log_n;
    GP22_MIS_part3_alg = new LubyMISAlg(node, part3_starting_round);
    GP22_MIS_part3_alg->max_num_rounds = part3_max_num_rounds;

    max_num_rounds = part3_starting_round + part3_max_num_rounds;

    EV << "part1_starting_round = " << part1_starting_round << ' '
       << GP22_MIS_part1_alg->starting_round << '\n';
    EV << "part1_2_starting_round = " << part1_2_starting_round << ' '
       << GP22_MIS_part1_2_alg->starting_round << '\n';
    EV << "part2_starting_round = " << part2_starting_round << ' '
       << GP22_MIS_part2_alg->starting_round << '\n';
    EV << "part3_starting_round = " << part3_starting_round << ' '
       << GP22_MIS_part3_alg->starting_round << '\n';
    EV << "total max_num_rounds = " << max_num_rounds << '\n';
}

void GP22MISAlg::handle_message(cMessage *msg)
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
    EV << "GP22_MIS_stage = " << GP22_MIS_stage << '\n';

    switch (GP22_MIS_stage) {
    case GP22MISStage::PART1: {
        if (node->id <= threshold) {
            call_handle_message(GP22_MIS_part1_alg, msg);
        } else {
            delete msg;
        }
        break;
    }
    case GP22MISStage::PART1_2:
        call_handle_message(GP22_MIS_part1_2_alg, msg);
        break;
    case GP22MISStage::PART2:
        call_handle_message(GP22_MIS_part2_alg, msg);
        break;
    case GP22MISStage::PART3:
        call_handle_message(GP22_MIS_part3_alg, msg);
        break;
    default:
        break;
    }

    n_awake_rounds = GP22_MIS_part1_alg->n_awake_rounds +
                     GP22_MIS_part1_2_alg->n_awake_rounds +
                     GP22_MIS_part2_alg->n_awake_rounds +
                     GP22_MIS_part3_alg->n_awake_rounds;
    EV << "MIS_status after round #" << current_round_id << " = " << MIS_status
       << ": is_decided() = " << is_decided() << '\n';
    EV << "last_communication_round : " << last_communication_round << '\n';
    EV << "n_awake_rounds = " << n_awake_rounds << '\n';
    EV << "decided_round = " << decided_round << '\n';
}

void GP22MISAlg::stage_transition()
{
    EV << "MIS_status = " << MIS_status << '\n';
    if (GP22_MIS_stage == GP22MISStage::INITIAL_STAGE) {
        GP22_MIS_stage = GP22MISStage::PART1;
        for (int neighbor_id : node->all_neighbors) {
            if (neighbor_id <= threshold)
                GP22_MIS_part1_alg->all_remained_neighbors.insert(neighbor_id);
        }
        // do a tricky thing
        node->all_neighbors = std::vector<int>(all_remained_neighbors.begin(),
                                               all_remained_neighbors.end());
    } else if (current_round_id == part1_2_starting_round) {
        GP22_MIS_stage = GP22MISStage::PART1_2;
        for (int i = 0; i <= threshold; ++i) {
            all_remained_neighbors.erase(i);
        }
        GP22_MIS_part1_2_alg->all_remained_neighbors = all_remained_neighbors;
        GP22_MIS_part1_2_alg->MIS_status = MIS_status;
    } else if (current_round_id == part2_starting_round) {
        GP22_MIS_stage = GP22MISStage::PART2;
        node->all_neighbors = node->all_neighbors_original;
        GP22_MIS_part2_alg->init_alg_params(this);
    } else if (current_round_id == part3_starting_round) {
        GP22_MIS_stage = GP22MISStage::PART3;
        GP22_MIS_part3_alg->need_to_send = all_remained_neighbors;
        GP22_MIS_part3_alg->MIS_status = MIS_status;
    }
}

bool GP22MISAlg::is_selected() { return (MIS_status == IN_MIS); }

GP22MISAlg::~GP22MISAlg()
{
    delete GP22_MIS_part1_alg;
    delete GP22_MIS_part1_2_alg;
    delete GP22_MIS_part2_alg;
    delete GP22_MIS_part3_alg;
}
