#include <cassert>

#include "utils/utils.h"
#include "algorithms/mis/SW08_mis/SW08_MIS.h"
#include "algorithms/mis/SW08_mis/SW08_MIS_message.h"

void SW08MISAlg::set_alg_type() { EV << "SW08MIS::set_alg_type()\n"; alg_type = MIS_ALG; }

SW08MISAlg::SW08MISAlg(Node *node, int starting_round) {
    init(node, starting_round);
    EV << "SW08MISAlg::SW08MISAlg(Node *node, int starting_round)\n";
    n_competitions_per_phase = log_star(n_nodes) + 2;
    n_rounds_per_phase = N_ROUNDS_PER_COMPETITION * n_competitions_per_phase;
    n_rounds_per_stage = n_rounds_per_phase * N_PHASES_PER_STAGE;
    max_num_rounds = starting_round + n_rounds_per_stage * N_STAGES;
    EV << "\tSW08MISAlg::starting_round = " << starting_round << '\n';
    EV << "\tSW08MISAlg::n_competitions_per_phase = " << n_competitions_per_phase << '\n';
    EV << "\tSW08MISAlg::max_num_rounds = " << max_num_rounds << '\n';
}

void SW08MISAlg::init_alg_variables() {
    r = node->id;
    SW08_status = SW08_COMPETITOR;
    SW08_alg_round_type = SW08_EXCHANGING_R;
    for(int neighbor_id : node->all_neighbors) {
        undecided_neighbors.insert(neighbor_id);
        neighbors_r[neighbor_id] = neighbor_id;
        neighbors_status[neighbor_id] = SW08_COMPETITOR;
    }
}

void SW08MISAlg::stage_transition() {
    IAlgNode::stage_transition();
    current_round_alg_stage = SW08MISStage::MIS_STAGE;
    if (max_num_rounds && SW08_alg_round_type == SW08_EXCHANGING_R) {
        SW08_alg_round_type = SW08_EXCHANGING_STATE_1;
    } else if (SW08_alg_round_type == SW08_EXCHANGING_STATE_1) {
        SW08_alg_round_type = SW08_EXCHANGING_STATE_2;
    } else if (SW08_alg_round_type == SW08_EXCHANGING_STATE_2) {
        SW08_alg_round_type = SW08_EXCHANGING_R;
    }
}

bool SW08MISAlg::is_new_stage() {
    if (SW08_status == SW08_RULER || SW08_status == SW08_COMPETITOR) {
        EV << "Can't start new stage. Node" << node->id << " is " << SW08_status << "\n";
        return false;
    }
    bool res = true;
    for(auto it : neighbors_status) {
        SW08NodeStatus neighbor_status = it.second;
        if (is_decided_status(neighbor_status)) continue;
        if (neighbor_status == SW08_RULER || neighbor_status == SW08_COMPETITOR) {
            res = false;
            EV << "Can't start new stage. Neighbor" << it.first << " is " << neighbor_status << "\n";
            break;
        }
    }
    if (res) return true;
    return ((current_round_id - starting_round) % n_rounds_per_stage) == 0;
}

bool SW08MISAlg::is_new_phase() {
    if (SW08_status == SW08_COMPETITOR) {
        EV << "Can't start new phase. Node" << node->id << " is SW08_COMPETITOR\n";
        return false;
    }
    bool res = true;
    for(auto it : neighbors_status) {
        SW08NodeStatus neighbor_status = it.second;
        if (is_decided_status(neighbor_status)) continue;
        if (neighbor_status == SW08_COMPETITOR) {
            EV << "Can't start new phase. Neighbor " << it.first << " is SW08_COMPETITOR\n";
            res = false;
            break;
        }
    }
    if (res) return true;
    return ((current_round_id - starting_round) % n_rounds_per_phase) == 0;
}

bool SW08MISAlg::reset_stage_if_needed() {
    if (is_decided()) return false;
    if (is_new_stage()) {
        if (SW08_status == SW08_COMPETITOR || SW08_status == SW08_RULER) {
            EV_ERROR << "New stage but node" << node->id << " has status " << status << '\n';
        }

        SW08_status = SW08_COMPETITOR;
        r = node->id;
        EV << "New stage. Competitors = [" << node->id << ",";
        for(int neighbor_id : node->all_neighbors) {
            if (!is_decided_status(neighbors_status[neighbor_id])) {
                undecided_neighbors.insert(neighbor_id);
                neighbors_status[neighbor_id] = SW08_COMPETITOR;
                neighbors_r[neighbor_id] = neighbor_id;
                EV << neighbor_id << ",";
            }
        }
        EV << "]\n";
        return true;
    }
    return false;
}

bool SW08MISAlg::reset_phase_if_needed() {
    if (is_decided()) return false;
    if (!is_new_stage() && is_new_phase()) {
        if (SW08_status == SW08_COMPETITOR) {
            EV_ERROR << "New phase but node" << node->id << " has status " << status << '\n';
        }

        if (SW08_status == SW08_RULER) SW08_status = SW08_COMPETITOR;
        r = node->id;
        for(int neighbor_id : node->all_neighbors) {
            if (neighbors_status[neighbor_id] == SW08_RULER) {
                neighbors_status[neighbor_id] = SW08_COMPETITOR;
                neighbors_r[neighbor_id] = neighbor_id;
            }
        }

        std::vector<int> competitors;
        if (SW08_status == SW08_COMPETITOR) competitors.push_back(node->id);
        for(int neighbor_id : node->all_neighbors) {
            if (neighbors_status[neighbor_id] == SW08_COMPETITOR) {
                competitors.push_back(neighbor_id);
            }
        }
        EV << "New phase. Competitors = [";
        for(int competitor : competitors) EV << competitor << ",";
        EV << "]\n";
        return true;
    }
    return false;
}

bool SW08MISAlg::is_decided_status(SW08NodeStatus node_status) {
    return (node_status == SW08_DOMINATOR) || (node_status == SW08_DOMINATED);
}

void SW08MISAlg::update_previous_status() {
    SW08_previous_status = SW08_status;
}

int SW08MISAlg::find_smallest_r() {
    EV << "SW08MISAlg::find_smallest_r(): (" << node->id << "," << r << "): ";
    int smallest_r = INT_MAX;
    for(int neighbor_id : undecided_neighbors) {
        if (is_decided_status(neighbors_status[neighbor_id])) {
            EV_ERROR << "Node" << neighbor_id << " is decided: " << neighbors_status[neighbor_id] << '\n';
        }
        //EV << "(" << neighbor_id << "," << neighbors_r[neighbor_id] << "," << neighbors_status[neighbor_id] << "), ";
        if (neighbors_status[neighbor_id] != SW08_COMPETITOR) continue;
        smallest_r = std::min(smallest_r, neighbors_r[neighbor_id]);
        EV << "(" << neighbor_id << "," << neighbors_r[neighbor_id] << "," << neighbors_status[neighbor_id] << "), ";
    }
    if (smallest_r == INT_MAX) smallest_r = 0;
    EV << '\n';
    EV << "smallest_r = " << smallest_r << '\n';
    return smallest_r;
}

cMessage *SW08MISAlg::process_message_queue() {
    EV << "\t" << "SW08MISAlg::process_message_queue()\n";
    if (SW08_alg_round_type == SW08_EXCHANGING_R) {
        return process_message_queue_for_exchange_r_round();
    } else if (SW08_alg_round_type == SW08_EXCHANGING_STATE_1) {
        return process_message_queue_for_exchange_state_1_round();
    } else {
        return process_message_queue_for_exchange_state_2_round();
    }
    if (!is_decided()) status = UNDECIDED;
    else {
        if (SW08_status == SW08_DOMINATOR) status = IN_MIS;
        else status = NOT_IN_MIS;
    }
    return nullptr;
}

void SW08MISAlg::set_need_to_send_to_competitor_neighbors() {
    need_to_send.clear();
    for(auto it : neighbors_status) {
        int neighbor_id = it.first;
        SW08NodeStatus neighbor_status = it.second;
        if (neighbor_status == SW08_COMPETITOR) {
            need_to_send.insert(neighbor_id);
        }
    }
}

void SW08MISAlg::set_need_to_send_to_undecided_neighbors() {
    need_to_send.clear();
    for(auto it : neighbors_status) {
        int neighbor_id = it.first;
        SW08NodeStatus neighbor_status = it.second;
        if (!is_decided_status(neighbor_status)) {
            need_to_send.insert(neighbor_id);
        }
    }
}

cMessage *SW08MISAlg::process_message_queue_for_exchange_r_round() {
    EV << "SW08MISAlg::process_message_queue_for_exchange_r_round() -- round " << current_round_id << "\n";
    bool new_stage = false;
    bool new_phase = false;
    if (current_round_id == starting_round) {
        init_alg_variables();
    } else {
        new_stage = reset_stage_if_needed();
        if (!new_stage) new_phase = reset_phase_if_needed();
    }

    EV << "\t" << "Process message queue\n";
    for(cMessage *msg : message_queue) {
        SW08MISMessage *SW08_MIS_message = dynamic_cast<SW08MISMessage *>(msg);
        int neighbor_id = SW08_MIS_message->getSenderId();
        SW08NodeStatus neighbor_status = SW08_MIS_message->getStatus();
        if (is_decided_status(neighbor_status)) {
            neighbors_status[neighbor_id] = neighbor_status;
            undecided_neighbors.erase(neighbor_id);
        }
        if (!new_phase && !new_stage) neighbors_status[neighbor_id] = neighbor_status;
        EV << "\t\t" << "neighbor_id = " << neighbor_id << ' ' << "neighbor_status = " << neighbor_status << '\n';
    }
    EV << "\t" << "Done processing message queue. Now finding smallest r\n";
    if (SW08_status == SW08_COMPETITOR) {
        int smallest_r = find_smallest_r();
        r = find_highest_greater_bit(r, smallest_r);
        SW08MISMessage *new_message = new SW08MISMessage("SW08_exchange_r");
        new_message->setR(r);
        new_message->setSenderId(node->id);
        set_need_to_send_to_competitor_neighbors();
        return new_message;
    }
    return nullptr;
}

cMessage *SW08MISAlg::process_message_queue_for_exchange_state_1_round() {
    EV << "SW08MISAlg::process_message_queue_for_exchange_state_1_round() -- round " << current_round_id << "\n";
    if (SW08_status == SW08_COMPETITOR) {
        bool can_be_dominator = true;
        bool can_be_ruler = true;
        EV << "\t" << "r = " << r << '\n';
        for(cMessage *msg : message_queue) {
            SW08MISMessage *SW08_MIS_message = dynamic_cast<SW08MISMessage *>(msg);
            int neighbor_id = SW08_MIS_message->getSenderId();
            int neighbor_r = SW08_MIS_message->getR();
            neighbors_r[neighbor_id] = neighbor_r;
            EV << "\t" << "neighbor_id = " << neighbor_id << ' ' << "neighbor_r = " << neighbor_r << '\n';
            if (neighbor_r <= r) can_be_dominator = false;
            if (neighbor_r < r) can_be_ruler = false;
        }
        if (can_be_dominator) {
            SW08_status = SW08_DOMINATOR;
            dominator = node->id;
            last_communication_round = current_round_id + 1;
            EV << "can_be_dominator" << '\n';
        }
        else if (can_be_ruler) SW08_status = SW08_RULER;
    }
    EV << "SW08_status = " << SW08_status << '\n';
    SW08MISMessage *new_message = new SW08MISMessage("SW08_exchange_state_1");
    new_message->setStatus(SW08_status);
    new_message->setSenderId(node->id);
    set_need_to_send_to_undecided_neighbors();
    return new_message;
}

cMessage *SW08MISAlg::process_message_queue_for_exchange_state_2_round() {
    EV << "SW08MISAlg::process_message_queue_for_exchange_state_2_round() -- round " << current_round_id << "\n";
    for(cMessage *msg : message_queue) {
        SW08MISMessage *SW08_MIS_message = dynamic_cast<SW08MISMessage *>(msg);
        int neighbor_id = SW08_MIS_message->getSenderId();
        SW08NodeStatus neighbor_status = SW08_MIS_message->getStatus();
        neighbors_status[neighbor_id] = neighbor_status;
        EV << "\t" << "neighbor_id = " << neighbor_id << ' ' << "neighbor_status = " << neighbor_status << '\n';
        if (is_decided_status(neighbor_status)) undecided_neighbors.erase(neighbor_id);
        if (neighbor_status == SW08_DOMINATOR) {
            SW08_status = SW08_DOMINATED;
            dominator = neighbor_id;
            last_communication_round = current_round_id;
            break;
        } else if (SW08_status != SW08_RULER && neighbor_status == SW08_RULER) {
            SW08_status = SW08_RULED;
        }
    }
    EV << "SW08_status = " << SW08_status << '\n';
    SW08MISMessage *new_message = new SW08MISMessage("SW08_exchange_state_1");
    new_message->setStatus(SW08_status);
    new_message->setSenderId(node->id);
    set_need_to_send_to_undecided_neighbors();
    return new_message;
}

bool SW08MISAlg::is_selected() {
    return (SW08_status == SW08_DOMINATOR);
}

bool SW08MISAlg::is_decided() {
    return is_decided_status(SW08_status);
}

bool SW08MISAlg::is_awake() {
    if (current_round_id == starting_round) {
        awake_round_map[current_round_id] = true;
        return true;
    }
    return IAlgNode::is_awake();
}

void SW08MISAlg::record_decided_round() {
    if (!is_decided_status(SW08_previous_status) && is_decided_status(SW08_status)) {
        decided_round = current_round_id;
    }
}
