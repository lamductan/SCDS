#include <cassert>

#include "utils/utils.h"
#include "algorithms/mis/KW08_mis/KW08_MIS.h"
#include "algorithms/mis/KW08_mis/KW08_MIS_message.h"

void KW08MISAlg::set_alg_type() { EV << "KW08MIS::set_alg_type()\n"; alg_type = MIS_ALG; }

KW08MISAlg::KW08MISAlg(Node *node, int starting_round) {
    init(node, starting_round);
    EV << "KW08MISAlg::KW08MISAlg(Node *node, int starting_round)\n";
    n_competitions_per_phase = log_star(n_nodes) + 2;
    n_rounds_per_phase = N_ROUNDS_PER_COMPETITION * n_competitions_per_phase;
    n_rounds_per_stage = n_rounds_per_phase * N_PHASES_PER_STAGE;
    max_num_rounds = starting_round + n_rounds_per_stage * N_STAGES;
    EV << "starting_round = " << starting_round << '\n';
    EV << "n_competitions_per_phase = " << n_competitions_per_phase << '\n';
    EV << "max_num_rounds = " << max_num_rounds << '\n';
}

void KW08MISAlg::init_alg_variables() {
    r = node->id;
    KW08_status = KW08_COMPETITOR;
    KW08_alg_round_type = KW08_EXCHANGING_R;
    for(int neighbor_id : node->all_neighbors) {
        undecided_neighbors.insert(neighbor_id);
        neighbors_r[neighbor_id] = neighbor_id;
        neighbors_status[neighbor_id] = KW08_COMPETITOR;
    }
}

void KW08MISAlg::stage_transition() {
    current_round_alg_stage = KW08MISStage::MIS_STAGE;
    if (current_round_id < max_num_rounds && KW08_alg_round_type == KW08_EXCHANGING_R) {
        KW08_alg_round_type = KW08_EXCHANGING_STATE_1;
    } else if (current_round_id < max_num_rounds && KW08_alg_round_type == KW08_EXCHANGING_STATE_1) {
        KW08_alg_round_type = KW08_EXCHANGING_STATE_2;
    } else if (current_round_id < max_num_rounds && KW08_alg_round_type == KW08_EXCHANGING_STATE_2) {
        KW08_alg_round_type = KW08_EXCHANGING_R;
    } else {
        current_round_alg_stage = KW08MISStage::END_STAGE;
    }
}

bool KW08MISAlg::is_new_stage() {
    //if (status == KW08_RULER || status == KW08_COMPETITOR) return false;
    bool res = true;
    for(auto it : neighbors_status) {
        KW08NodeStatus neighbor_status = it.second;
        if (is_decided_status(neighbor_status)) continue;
        if (neighbor_status == KW08_RULER || neighbor_status == KW08_COMPETITOR) {
            res = false;
            break;
        }
    }
    if (res) return true;
    return ((current_round_id - starting_round) % n_rounds_per_stage) == 0;
}

bool KW08MISAlg::is_new_phase() {
    if (KW08_status == KW08_COMPETITOR) {
        EV << "Can't start new phase. Node" << node->id << " is KW08_COMPETITOR\n";
        return false;
    }
    bool res = true;
    for(auto it : neighbors_status) {
        KW08NodeStatus neighbor_status = it.second;
        if (is_decided_status(neighbor_status)) continue;
        if (neighbor_status == KW08_COMPETITOR) {
            EV << "Can't start new phase. Neighbor " << it.first << " is KW08_COMPETITOR\n";
            res = false;
            break;
        }
    }
    if (res) return true;
    return ((current_round_id - starting_round) % n_rounds_per_phase) == 0;
}

bool KW08MISAlg::reset_stage_if_needed() {
    if (is_decided()) return false;
    if (is_new_stage()) {
        if (KW08_status == KW08_COMPETITOR || KW08_status == KW08_RULER) {
            EV_ERROR << "New stage but node" << node->id << " has status " << status << '\n';
        }

        KW08_status = KW08_COMPETITOR;
        r = node->id;
        EV << "New stage. Competitors = [" << node->id << ",";
        for(int neighbor_id : node->all_neighbors) {
            if (!is_decided_status(neighbors_status[neighbor_id])) {
                undecided_neighbors.insert(neighbor_id);
                neighbors_status[neighbor_id] = KW08_COMPETITOR;
                neighbors_r[neighbor_id] = neighbor_id;
                EV << neighbor_id << ",";
            }
        }
        EV << "]\n";
        return true;
    }
    return false;
}

bool KW08MISAlg::reset_phase_if_needed() {
    if (is_decided()) return false;
    if (!is_new_stage() && is_new_phase()) {
        if (KW08_status == KW08_COMPETITOR) {
            EV_ERROR << "New phase but node" << node->id << " has status " << status << '\n';
        }

        if (KW08_status == KW08_RULER) KW08_status = KW08_COMPETITOR;
        r = node->id;
        for(int neighbor_id : node->all_neighbors) {
            if (neighbors_status[neighbor_id] == KW08_RULER) {
                neighbors_status[neighbor_id] = KW08_COMPETITOR;
                neighbors_r[neighbor_id] = neighbor_id;
            }
        }

        std::vector<int> competitors;
        if (KW08_status == KW08_COMPETITOR) competitors.push_back(node->id);
        for(int neighbor_id : node->all_neighbors) {
            if (neighbors_status[neighbor_id] == KW08_COMPETITOR) {
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

bool KW08MISAlg::is_decided_status(KW08NodeStatus node_status) {
    return (node_status == KW08_DOMINATOR) || (node_status == KW08_DOMINATED);
}

void KW08MISAlg::update_previous_status() {
    KW08_previous_status = KW08_status;
}

int KW08MISAlg::find_smallest_r() {
    EV << "KW08MISAlg::find_smallest_r(): (" << node->id << "," << r << "): ";
    int smallest_r = INT_MAX;
    for(int neighbor_id : undecided_neighbors) {
        if (is_decided_status(neighbors_status[neighbor_id])) {
            EV_ERROR << "Node" << neighbor_id << " is decided: " << neighbors_status[neighbor_id] << '\n';
        }
        //EV << "(" << neighbor_id << "," << neighbors_r[neighbor_id] << "," << neighbors_status[neighbor_id] << "), ";
        if (neighbors_status[neighbor_id] != KW08_COMPETITOR) continue;
        smallest_r = std::min(smallest_r, neighbors_r[neighbor_id]);
        EV << "(" << neighbor_id << "," << neighbors_r[neighbor_id] << "," << neighbors_status[neighbor_id] << "), ";
    }
    if (smallest_r == INT_MAX) smallest_r = 0;
    EV << '\n';
    EV << "smallest_r = " << smallest_r << '\n';
    return smallest_r;
}

cMessage *KW08MISAlg::process_message_queue() {
    EV << "\t" << "KW08MISAlg::process_message_queue()\n";
    if (KW08_alg_round_type == KW08_EXCHANGING_R) {
        return process_message_queue_for_exchange_r_round();
    } else if (KW08_alg_round_type == KW08_EXCHANGING_STATE_1) {
        return process_message_queue_for_exchange_state_1_round();
    } else {
        return process_message_queue_for_exchange_state_2_round();
    }
    if (!is_decided()) status = UNDECIDED;
    else {
        if (KW08_status == KW08_DOMINATOR) status = IN_MIS;
        else status = NOT_IN_MIS;
    }
    return nullptr;
}

void KW08MISAlg::set_need_to_send_to_competitor_neighbors() {
    need_to_send.clear();
    for(auto it : neighbors_status) {
        int neighbor_id = it.first;
        KW08NodeStatus neighbor_status = it.second;
        if (neighbor_status == KW08_COMPETITOR) {
            need_to_send.insert(neighbor_id);
        }
    }
}

void KW08MISAlg::set_need_to_send_to_undecided_neighbors() {
    need_to_send.clear();
    for(auto it : neighbors_status) {
        int neighbor_id = it.first;
        KW08NodeStatus neighbor_status = it.second;
        if (!is_decided_status(neighbor_status)) {
            need_to_send.insert(neighbor_id);
        }
    }
}

cMessage *KW08MISAlg::process_message_queue_for_exchange_r_round() {
    EV << "KW08MISAlg::process_message_queue_for_exchange_r_round() -- round " << current_round_id << "\n";
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
        KW08MISMessage *KW08_MIS_message = dynamic_cast<KW08MISMessage *>(msg);
        int neighbor_id = KW08_MIS_message->getSenderId();
        KW08NodeStatus neighbor_status = KW08_MIS_message->getStatus();
        if (!new_phase && !new_stage) neighbors_status[neighbor_id] = neighbor_status;
        EV << "\t\t" << "neighbor_id = " << neighbor_id << ' ' << "neighbor_status = " << neighbor_status << '\n';
        if (is_decided_status(neighbor_status)) undecided_neighbors.erase(neighbor_id);
    }
    EV << "\t" << "Done processing message queue. Now finding smallest r\n";
    if (KW08_status == KW08_COMPETITOR) {
        int smallest_r = find_smallest_r();
        r = find_highest_greater_bit(r, smallest_r);
        KW08MISMessage *new_message = new KW08MISMessage("KW08_exchange_r");
        new_message->setR(r);
        new_message->setSenderId(node->id);
        set_need_to_send_to_competitor_neighbors();
        return new_message;
    }
    return nullptr;
}

cMessage *KW08MISAlg::process_message_queue_for_exchange_state_1_round() {
    EV << "KW08MISAlg::process_message_queue_for_exchange_state_1_round() -- round " << current_round_id << "\n";
    if (KW08_status == KW08_COMPETITOR) {
        bool can_be_dominator = true;
        bool can_be_ruler = true;
        EV << "\t" << "r = " << r << '\n';
        for(cMessage *msg : message_queue) {
            KW08MISMessage *KW08_MIS_message = dynamic_cast<KW08MISMessage *>(msg);
            int neighbor_id = KW08_MIS_message->getSenderId();
            int neighbor_r = KW08_MIS_message->getR();
            neighbors_r[neighbor_id] = neighbor_r;
            EV << "\t" << "neighbor_id = " << neighbor_id << ' ' << "neighbor_r = " << neighbor_r << '\n';
            if (neighbor_r <= r) can_be_dominator = false;
            if (neighbor_r < r) can_be_ruler = false;
        }
        if (can_be_dominator) {
            KW08_status = KW08_DOMINATOR;
            last_communication_round = current_round_id + 1;
            EV << "can_be_dominator" << '\n';
        }
        else if (can_be_ruler) KW08_status = KW08_RULER;
    }
    EV << "KW08_status = " << KW08_status << '\n';
    KW08MISMessage *new_message = new KW08MISMessage("KW08_exchange_state_1");
    new_message->setStatus(KW08_status);
    new_message->setSenderId(node->id);
    set_need_to_send_to_undecided_neighbors();
    return new_message;
}

cMessage *KW08MISAlg::process_message_queue_for_exchange_state_2_round() {
    EV << "KW08MISAlg::process_message_queue_for_exchange_state_2_round() -- round " << current_round_id << "\n";
    for(cMessage *msg : message_queue) {
        KW08MISMessage *KW08_MIS_message = dynamic_cast<KW08MISMessage *>(msg);
        int neighbor_id = KW08_MIS_message->getSenderId();
        KW08NodeStatus neighbor_status = KW08_MIS_message->getStatus();
        neighbors_status[neighbor_id] = neighbor_status;
        EV << "\t" << "neighbor_id = " << neighbor_id << ' ' << "neighbor_status = " << neighbor_status << '\n';
        if (is_decided_status(neighbor_status)) undecided_neighbors.erase(neighbor_id);
        if (neighbor_status == KW08_DOMINATOR) {
            KW08_status = KW08_DOMINATED;
            last_communication_round = current_round_id;
            break;
        } else if (KW08_status != KW08_RULER && neighbor_status == KW08_RULER) {
            KW08_status = KW08_RULED;
        }
    }
    EV << "KW08_status = " << KW08_status << '\n';
    KW08MISMessage *new_message = new KW08MISMessage("KW08_exchange_state_1");
    new_message->setStatus(KW08_status);
    new_message->setSenderId(node->id);
    set_need_to_send_to_undecided_neighbors();
    return new_message;
}

bool KW08MISAlg::is_selected() {
    return (KW08_status == KW08_DOMINATOR);
}

bool KW08MISAlg::is_decided() {
    return is_decided_status(KW08_status);
}

void KW08MISAlg::record_decided_round() {
    if (!is_decided_status(KW08_previous_status) && is_decided_status(KW08_status)) {
        decided_round = current_round_id;
    }
}
