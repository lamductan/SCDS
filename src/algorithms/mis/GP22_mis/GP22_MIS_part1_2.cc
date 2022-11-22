#include "algorithms/mis/GP22_mis/GP22_MIS_part1_2.h"
#include "algorithms/mis/GP22_mis/GP22_MIS_message.h"

void GP22MISPart1_2_Alg::set_alg_type() { EV << "GP22MISPart1_2_::set_alg_type()\n"; alg_type = MIS_ALG; }

GP22MISPart1_2_Alg::GP22MISPart1_2_Alg(Node *node, int starting_round) {
    init(node, starting_round);
}

cMessage *GP22MISPart1_2_Alg::process_message_queue() {
    EV << "GP22MISPart1_2_Alg::process_message_queue()\n";

    for(cMessage *msg : message_queue) {
        GP22MISMessage *GP22_MIS_message = dynamic_cast<GP22MISMessage *>(msg);
        int neighbor_id = GP22_MIS_message->getSenderId();
        MISNodeStatus neighbor_status = GP22_MIS_message->getStatus();
        EV << "\t" << "neighbor_id = " << neighbor_id << ", neighbor_status = " << neighbor_status << '\n';
        if (neighbor_status == IN_MIS) {
            if (MIS_status == UNDECIDED) {
                MIS_status = NOT_IN_MIS;
                dominator = neighbor_id;
            }
            all_remained_neighbors.erase(neighbor_id);
        } else if (neighbor_status == NOT_IN_MIS) {
            all_remained_neighbors.erase(neighbor_id);
        }
    }

    GP22MISMessage *new_message = new GP22MISMessage("GP22MISPart1_2");
    new_message->setSenderId(node->id);
    new_message->setStatus(MIS_status);
    clear_message_queue();
    need_to_send = all_remained_neighbors;
    return new_message;
}

bool GP22MISPart1_2_Alg::is_selected() {
    return (MIS_status == IN_MIS);
}

bool GP22MISPart1_2_Alg::is_awake() {
    EV << "GP22MISPart1_2_Alg::is_awake():\n";
    EV << "\tcurrent_round_id = " << current_round_id << '\n';
    EV << "\tstarting_round = " << starting_round << '\n';
    EV << "\tis_decided() = " << is_decided() << '\n';
    if ((current_round_id == starting_round) || !is_decided()) {
        awake_round_map[current_round_id] = true;
        return true;
    }
    return false;
}