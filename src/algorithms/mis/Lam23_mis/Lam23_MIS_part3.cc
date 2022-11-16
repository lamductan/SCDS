#include "algorithms/mis/Lam23_mis/Lam23_MIS_part3.h"

void LamMISPart3Alg::set_alg_type() { EV << "LamMISPart3Alg::set_alg_type()\n"; alg_type = MIS_ALG; }

LamMISPart3Alg::LamMISPart3Alg(Node *node, int starting_round) {
    init(node, starting_round);
    all_remained_neighbors = std::set<int>(node->all_neighbors.begin(), node->all_neighbors.end());
}

cMessage *LamMISPart3Alg::process_message_queue() {
    EV << "LamMISPart3Alg::process_message_queue()\n";
    for(cMessage *msg : message_queue) {
        LamMISMessage *Lam_MIS_message = dynamic_cast<LamMISMessage *>(msg);
        int neighbor_id = Lam_MIS_message->getSenderId();
        int neighbor_color = Lam_MIS_message->getColor();
        NodeStatus neighbor_status = Lam_MIS_message->getStatus();
        EV << "\t" << "neighbor_id = " << neighbor_id << ", neighbor_color = " << neighbor_color 
           << ", neighbor_status = " << neighbor_status << '\n';
        if (neighbor_status != UNDECIDED) {
            all_remained_neighbors.erase(neighbor_id);
            neighbor_colors.erase({neighbor_color, neighbor_id});
        }
        if (neighbor_status == IN_MIS) {
            if (status == UNDECIDED) {
                status = NOT_IN_MIS;
                dominator = neighbor_id;
            }
        }
    }

    EV << "LamMISPart3Alg::status = " << status << "\n";
    std::pair<int,int> color_id_pair = {color, node->id};
    if (status == UNDECIDED) {
        EV << "\t(color,id) = " << "(" << color_id_pair.first << "," << color_id_pair.second << ")"
           << " --- LamMISPart3Alg::neighbor_colors = [";
        for(auto it : neighbor_colors) EV << "(" << it.first << "," << it.second << "), ";
        EV << "]\n";
    }

    if (current_round_id > starting_round && status == UNDECIDED) {
        if (neighbor_colors.empty()) {
            status = IN_MIS;
            dominator = node->id;
        } else if (color_id_pair < *neighbor_colors.begin()) {
            status = IN_MIS;
            dominator = node->id;
        }
    }

    if (status == UNDECIDED) return nullptr;
    if (status == IN_MIS) EV << "\tJoin MIS\n";
    else EV << "\tNOT JOIN mis\n";
    LamMISMessage *new_message = new LamMISMessage("LamMISPart3Alg");
    new_message->setSenderId(node->id);
    new_message->setColor(color);
    new_message->setStatus(status);
    need_to_send = all_remained_neighbors;
    return new_message;
}

bool LamMISPart3Alg::is_selected() {
    return (status == IN_MIS);
}

bool LamMISPart3Alg::is_awake() {
    if (current_round_id == starting_round) {
        awake_round_map[current_round_id] = true;
        return true;
    }
    return IAlgNode::is_awake();
}