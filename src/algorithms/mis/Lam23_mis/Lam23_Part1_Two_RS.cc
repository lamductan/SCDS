#include "algorithms/mis/Lam23_mis/Lam23_MIS_message.h"

void LamTwoRSAlg::set_alg_type() { EV << "LamTwoRSAlg::set_alg_type()\n"; alg_type = MIS_ALG; }

LamTwoRSAlg::LamTwoRSAlg(Node *node, int starting_round) {
    init(node, starting_round);
}

void LamTwoRSAlg::stage_transition() {
    previous_round_alg_stage = current_round_alg_stage;
    current_round_alg_stage = LamTwoRSStage::LAM_TWO_RS_STAGE;
    if (current_round_id < max_num_rounds && Lam_Two_RS_alg_round_type == LAM_TWO_RS_GENERATING_MARK) {
        Lam_Two_RS_alg_round_type = LAM_TWO_RS_INFORMING_STATUS_1;
    } else if (current_round_id < max_num_rounds && Lam_Two_RS_alg_round_type == LAM_TWO_RS_INFORMING_STATUS_1) {
        Lam_Two_RS_alg_round_type = LAM_TWO_RS_INFORMING_STATUS_2;
    } else if (current_round_id < max_num_rounds && Lam_Two_RS_alg_round_type == LAM_TWO_RS_INFORMING_STATUS_2) {
        Lam_Two_RS_alg_round_type = LAM_TWO_RS_GENERATING_MARK;
    } else {
        current_round_alg_stage = LamTwoRSStage::END_STAGE;
    }
}

cMessage *LamTwoRSAlg::process_message_queue() {
    EV << "\t" << "LamTwoRSAlg::process_message_queue()\n";
    previous_status = status;
    if (is_decided()) {
        EV << "\t\t" << "Status is " << status << " decided at round " << last_communication_round << "\n";
        return nullptr;
    }

    // Init the number of undecided neighbors to be the number of neighbors.
    // Only do this once.
    if (previous_round_alg_stage == LamTwoRSStage::INITIAL_STAGE
            && current_round_alg_stage == LamTwoRSStage::LAM_TWO_RS_STAGE) {
        degree = node->all_neighbors.size();
    }

    if (Lam_Two_RS_alg_round_type == LAM_TWO_RS_GENERATING_MARK) {
        return process_message_queue_for_generate_mark_round();
    } else if (Lam_Two_RS_alg_round_type == LAM_TWO_RS_INFORMING_STATUS_1) {
        return process_message_queue_for_informing_status_1_round();
    } else if (Lam_Two_RS_alg_round_type == LAM_TWO_RS_INFORMING_STATUS_2) {
        return process_message_queue_for_informing_status_2_round();
    }
    return nullptr;
}

cMessage *LamTwoRSAlg::process_message_queue_for_generate_mark_round() {
    EV << "LamTwoRSAlg::process_message_queue_for_generate_mark_round()\n";
    for(cMessage *msg : message_queue) {
        LamMISMessage *Lam_MIS_message = dynamic_cast<LamMISMessage *>(msg);
        int neighbor_id = Lam_MIS_message->getSenderId();
        LamTwoRSNodeStatus Lam_Two_RS_neighbor_status = Lam_MIS_message->getTwoRSStatus();
        if (Lam_Two_RS_neighbor_status != LAM_TWO_RS_UNDECIDED) need_to_send.erase(neighbor_id);
        if (Lam_Two_RS_neighbor_status == LAM_TWO_RS_1_HOP) {
            Lam_Two_RS_status = LAM_TWO_RS_2_HOP;
            parent = neighbor_id;
            cluster_center_id = Lam_MIS_message->getClusterCenterId();
            break;
        }
    }
    
    marked = node->bernoulli(1.0/(2*degree));
    LamMISMessage *new_message = new LamMISMessage("LamMISMessage_2RS");
    new_message->setSenderId(node->id);
    new_message->setMarked(marked);
    new_message->setDegree(degree);
    new_message->setTwoRSStatus(Lam_Two_RS_status);
    EV << "\t\t" << "marked = " << marked << ", degree = " << degree << '\n';
    return new_message;
}

cMessage *LamTwoRSAlg::process_message_queue_for_informing_status_1_round() {
    EV << "LamTwoRSAlg::process_message_queue_for_informing_status_1_round()\n";
    degree = message_queue.size();
    if (degree == 0) {
        EV << "\t\t" << "degree = 0 --> become 2RS cluster center\n";
        Lam_Two_RS_status = LAM_TWO_RS_CLUSTER_CENTER;
        return nullptr;
    } else {
        EV << "\t\t" << "degree = " << degree << ", marked = 1 --> find higher priority message\n";
        need_to_send.clear();
        for(cMessage *msg : message_queue) {
            LamMISMessage *Lam_MIS_message = dynamic_cast<LamMISMessage *>(msg);
            int neighbor_id = Lam_MIS_message->getSenderId();
            bool neighbor_marked = Lam_MIS_message->getMarked();
            int neighbor_degree = Lam_MIS_message->getDegree();
            LamTwoRSNodeStatus Lam_Two_RS_neighbor_status = Lam_MIS_message->getTwoRSStatus();
            EV << "\t\t\t" << "neighbor_id = " << neighbor_id << ", neighbor_marked = " << neighbor_marked
                << ", neighbor_degree = " << neighbor_degree
                << ", Lam_Two_RS_neighbor_status = " << Lam_Two_RS_neighbor_status <<'\n';
            if (Lam_Two_RS_neighbor_status != LAM_TWO_RS_UNDECIDED) {
                continue;
            }
            need_to_send.insert(neighbor_id);
            if (!neighbor_marked) continue;
            if ((neighbor_degree > degree) || (neighbor_degree == degree && neighbor_id > node->id)) {
                marked = false;
            }
        }
        if (!marked) return nullptr;
        Lam_Two_RS_status = LAM_TWO_RS_CLUSTER_CENTER;
        LamMISMessage *new_message = new LamMISMessage("LamMISMessage_2RS");
        new_message->setSenderId(node->id);
        new_message->setStatus(status);
        new_message->setTwoRSStatus(Lam_Two_RS_status);
        new_message->setClusterCenterId(node->id);
        return new_message;
    }
    return nullptr;
}

cMessage *LamTwoRSAlg::process_message_queue_for_informing_status_2_round() {
    EV << "LamTwoRSAlg::process_message_queue_for_informing_status_2_round()\n";
    need_to_send.clear();
    need_to_send.insert(-1);
    for(cMessage *msg : message_queue) {
        LamMISMessage *Lam_MIS_message = dynamic_cast<LamMISMessage *>(msg);
        int neighbor_id = Lam_MIS_message->getSenderId();
        LamTwoRSNodeStatus Lam_Two_RS_neighbor_status = Lam_MIS_message->getTwoRSStatus();
        EV << "\t\t\t" << "neighbor_id = " << neighbor_id
            << ", Lam_Two_RS_neighbor_status = " << Lam_Two_RS_neighbor_status <<'\n';
        if (Lam_Two_RS_neighbor_status == LAM_TWO_RS_CLUSTER_CENTER) {
            Lam_Two_RS_status = LAM_TWO_RS_1_HOP;
            cluster_center_id = Lam_MIS_message->getClusterCenterId();
            break;
        }
    }
    if (Lam_Two_RS_status == LAM_TWO_RS_1_HOP) {
        LamMISMessage *new_message = new LamMISMessage("LamMISMessage_2RS");
        new_message->setSenderId(node->id);
        new_message->setStatus(status);
        new_message->setTwoRSStatus(Lam_Two_RS_status);
        new_message->setClusterCenterId(cluster_center_id);
    return new_message;
    }
    return nullptr;
}

bool LamTwoRSAlg::is_selected() {
    return (Lam_Two_RS_status == LAM_TWO_RS_CLUSTER_CENTER);
}

bool LamTwoRSAlg::is_decided() {
    return (Lam_Two_RS_status != LAM_TWO_RS_UNDECIDED);
}

void LamTwoRSAlg::update_previous_status() {
    Lam_Two_RS_previous_status = Lam_Two_RS_status;
}

void LamTwoRSAlg::record_decided_round() {
    if (Lam_Two_RS_previous_status == LAM_TWO_RS_UNDECIDED && Lam_Two_RS_status != LAM_TWO_RS_UNDECIDED) {
        decided_round = current_round_id;
    }
}