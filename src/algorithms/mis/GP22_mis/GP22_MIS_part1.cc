#include "algorithms/mis/GP22_mis/GP22_MIS_part1.h"
#include "algorithms/mis/GP22_mis/GP22_MIS_message.h"

void GP22MISPart1Alg::set_alg_type() { EV << "GP22MISPart1::set_alg_type()\n"; alg_type = MIS_ALG; }

GP22MISPart1Alg::GP22MISPart1Alg(Node *node, int starting_round) {
    init(node, starting_round);
}

cMessage *GP22MISPart1Alg::process_message_queue() {
    previous_status = status;
    EV << "GP22MISPart1Alg::process_message_queue() -- message_queue.size() = " << message_queue.size() << '\n';
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
    if ((status == UNDECIDED) && (all_remained_neighbors.empty() || (node->id < *all_remained_neighbors.begin()))) {
        status = IN_MIS;
        dominator = node->id;
    }
    
    if (previous_status == UNDECIDED && status != UNDECIDED) {
        GP22MISMessage *new_message = new GP22MISMessage("GP22MISPart1");
        new_message->setSenderId(node->id);
        new_message->setStatus(status);
        need_to_send = std::set<int>(all_remained_neighbors.begin(), all_remained_neighbors.end());
        return new_message;
    }
    return nullptr;
}

bool GP22MISPart1Alg::is_selected() {
    return (status == IN_MIS);
}

bool GP22MISPart1Alg::is_awake() {
    if (!is_decided()) {
        awake_round_map[current_round_id] = true;
        return true;
    }
    return false;
}