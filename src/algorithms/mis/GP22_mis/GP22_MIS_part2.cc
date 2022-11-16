#include "algorithms/mis/GP22_mis/GP22_MIS_part2.h"
#include "algorithms/mis/GP22_mis/GP22_MIS.h"
#include "algorithms/mis/GP22_mis/GP22_MIS_message.h"

void GP22MISPart2Alg::set_alg_type() { EV << "GP22MISPart2::set_alg_type()\n"; alg_type = MIS_ALG; }

GP22MISPart2Alg::GP22MISPart2Alg(Node *node, int starting_round) {
    init(node, starting_round);
}

void GP22MISPart2Alg::init_alg_params(GP22MISAlg *GP22_MIS_alg) {
    C = GP22_MIS_alg->C;
    d = GP22_MIS_alg->d;
    log_d = GP22_MIS_alg->log_d;
    num_iterations = GP22_MIS_alg->part2_num_iterations;
    num_phases = GP22_MIS_alg->part2_num_phases;
    num_rounds_per_iteration = GP22_MIS_alg->part2_num_rounds_per_iteration;
    all_remained_neighbors = GP22_MIS_alg->all_remained_neighbors;
}

int GP22MISPart2Alg::find_first_awake_round_in_iteration() {
    EV << "GP22MISPart2Alg::find_first_awake_round_in_iteration()\n";
    EV << "log_d = " << log_d << '\n';
    int res = -1;
    int total_rounds_until_last_phase = 0;
    for(int i = 1; i < num_phases; ++i) {
        int n_rounds = C*pow(log_d - i, 2);
        double p = pow(2, i)/d;
        int first_awake_round = node->geometric(p);
        if (first_awake_round % 2 == 1) --first_awake_round;
        if (first_awake_round < n_rounds) {
            if (res == -1) res = first_awake_round + total_rounds_until_last_phase;
        }
        total_rounds_until_last_phase += n_rounds;
        EV << '\t' << i << ' ' << total_rounds_until_last_phase << ' ' << res << '\n';
    }
    if (res == -1) res = total_rounds_until_last_phase;
    return res;
}

bool GP22MISPart2Alg::is_awake() {
    if (current_round_id_in_iteration >= first_awake_round_in_iteration) {
        awake_round_map[current_round_id] = true;
        return true;
    }
    return false;
}

void GP22MISPart2Alg::stage_transition() {
    IAlgNode::stage_transition();
    current_round_id_in_iteration = (current_round_id_in_iteration + 1) % num_rounds_per_iteration;
    if (current_round_id_in_iteration == n_rounds_of_current_phase) {
        ++current_phase_id_in_iteration;
    }
    n_rounds_of_current_phase = C*pow(log_d - current_phase_id_in_iteration, 2);
    
    if (current_round_id_in_iteration == 0) {
        current_phase_id_in_iteration = 1;
        first_awake_round_in_iteration = find_first_awake_round_in_iteration();
    }
    
    if (current_round_id_in_iteration % 2 == 0) {
        GP22Part2_alg_round_type = GP22Part2_DECLARING_MARK;
    } else GP22Part2_alg_round_type = GP22Part2_DECLARING_STATUS;
}

cMessage *GP22MISPart2Alg::process_message_queue() {
    EV << "GP22MISPart2Alg::process_message_queue()\n";
    if (GP22Part2_alg_round_type == GP22Part2_DECLARING_STATUS) {
        return process_message_queue_declaring_status_round();
    } else {
        return process_message_queue_declaring_mark_round();
    }
}

cMessage *GP22MISPart2Alg::process_message_queue_declaring_mark_round() {
    EV << "GP22MISPart2Alg::process_message_queue_declaring_mark_round()\n";
    for(cMessage *msg : message_queue) {
        GP22MISMessage *GP22_MIS_message = dynamic_cast<GP22MISMessage *>(msg);
        int neighbor_id = GP22_MIS_message->getSenderId();
        NodeStatus neighbor_status = GP22_MIS_message->getStatus();
        EV << "\t" << "neighbor_id = " << neighbor_id << ", neighbor_status = " << neighbor_status << '\n';
        if (neighbor_status == IN_MIS) {
            if (status == UNDECIDED) {
                status = NOT_IN_MIS;
                dominator = neighbor_id;
            }
            all_remained_neighbors.erase(neighbor_id);
        } else if (neighbor_status == NOT_IN_MIS) {
            all_remained_neighbors.erase(neighbor_id);
        }
    }
    if (status == UNDECIDED && all_remained_neighbors.empty()) status = IN_MIS;
    if (status != UNDECIDED) marked = false;
    else marked = node->bernoulli(pow(2, current_phase_id_in_iteration)/log_d);
    GP22MISMessage *new_message = new GP22MISMessage("GP22MISPart2");
    new_message->setSenderId(node->id);
    new_message->setStatus(status);
    new_message->setMarked(marked);
    need_to_send = std::set<int>(all_remained_neighbors.begin(), all_remained_neighbors.end());
    return new_message;
}

cMessage *GP22MISPart2Alg::process_message_queue_declaring_status_round() {
    EV << "GP22MISPart2Alg::process_message_queue_declaring_status_round()\n";
    bool has_one_marked_neighbor = false;
    for(cMessage *msg : message_queue) {
        GP22MISMessage *GP22_MIS_message = dynamic_cast<GP22MISMessage *>(msg);
        int neighbor_id = GP22_MIS_message->getSenderId();
        NodeStatus neighbor_status = GP22_MIS_message->getStatus();
        bool neighbor_marked = GP22_MIS_message->getMarked();
        EV << "\t" << "neighbor_id = " << neighbor_id << ", neighbor_marked = " << neighbor_marked << '\n';
        if (neighbor_status == IN_MIS) {
            if (status == UNDECIDED) {
                status = NOT_IN_MIS;
                dominator = neighbor_id;
            }
            all_remained_neighbors.erase(neighbor_id);
        } else if (neighbor_status == NOT_IN_MIS) {
            all_remained_neighbors.erase(neighbor_id);
        }
        if (neighbor_marked) has_one_marked_neighbor = true;
    }
    if (!marked && status == UNDECIDED) return nullptr;
    if (marked && !has_one_marked_neighbor) {
        status = IN_MIS;
        dominator = node->id;
    }
    GP22MISMessage *new_message = new GP22MISMessage("GP22MISPart2");
    new_message->setSenderId(node->id);
    new_message->setStatus(status);
    new_message->setMarked(marked);
    need_to_send = std::set<int>(all_remained_neighbors.begin(), all_remained_neighbors.end());
    return new_message;
}

bool GP22MISPart2Alg::is_selected() {
    return (status == IN_MIS);
}