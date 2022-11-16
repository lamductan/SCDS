#include "algorithms/mis/Lam23_mis/Lam23_InformingNeighbors.h"
#include "algorithms/mis/Lam23_mis/Lam23_MIS_message.h"

void LamInformingNeighborsAlg::set_alg_type() { EV << "LamInformingNeighborsAlg::set_alg_type()\n"; alg_type = MIS_ALG; }

LamInformingNeighborsAlg::LamInformingNeighborsAlg(Node *node, int starting_round) {
    init(node, starting_round);
    max_num_rounds = 2;
}

cMessage *LamInformingNeighborsAlg::process_message_queue() {
    EV << "LamInformingNeighborsAlg::process_message_queue()\n";
    for(cMessage *msg : message_queue) {
        LamMISMessage *Lam_MIS_message = dynamic_cast<LamMISMessage *>(msg);
        int neighbor_id = Lam_MIS_message->getSenderId();
        int neighbor_color = Lam_MIS_message->getColor();
        int neighbor_cluster = Lam_MIS_message->getClusterCenterId();
        LamTwoRSNodeStatus Lam_Two_RS_neighbor_status = Lam_MIS_message->getTwoRSStatus();
        EV << "\t" << "neighbor_id = " << neighbor_id << ", neighbor_color = " << neighbor_color
           << ", neighbor_cluster = " << neighbor_cluster << '\n';
        if (neighbor_color != -1) neighbor_colors.insert({neighbor_color, neighbor_id});
        if (Lam_Two_RS_neighbor_status == LAM_TWO_RS_CLUSTER_CENTER || Lam_Two_RS_neighbor_status == LAM_TWO_RS_1_HOP)
            all_remained_neighbors.erase(neighbor_id);
    }

    LamMISMessage *new_message = new LamMISMessage("LamInformingNeighborsAlg");
    new_message->setSenderId(node->id);
    new_message->setColor(color);
    new_message->setClusterCenterId(two_rs_cluster_center_id);
    new_message->setTwoRSStatus(Lam_Two_RS_status);
    return new_message;
}

bool LamInformingNeighborsAlg::is_selected() {
    return false;
}

bool LamInformingNeighborsAlg::is_awake() {
    awake_round_map[current_round_id] = true;
    return true;
}