#include "algorithms/mis/Lam23_mis/Lam23_MIS_message.h"

void BGKO22TwoRSAlg::set_alg_type()
{
    EV << "BGKO22TwoRSAlg::set_alg_type()\n";
    alg_type = TWO_RS_ALG;
}

BGKO22TwoRSAlg::BGKO22TwoRSAlg(Node *node, int starting_round)
{
    init(node, starting_round);
    all_remained_neighbors =
        std::set<int>(node->all_neighbors.begin(), node->all_neighbors.end());
}

void BGKO22TwoRSAlg::stage_transition()
{
    IAlgNode::stage_transition();
    previous_round_alg_stage = current_round_alg_stage;
    current_round_alg_stage = BGKO22TwoRSStage::BGKO22_TWO_RS_STAGE;
    if (BGKO22_Two_RS_alg_round_type == BGKO22_TWO_RS_GENERATING_MARK) {
        BGKO22_Two_RS_alg_round_type = BGKO22_TWO_RS_INFORMING_STATUS_1;
    } else if (BGKO22_Two_RS_alg_round_type ==
               BGKO22_TWO_RS_INFORMING_STATUS_1) {
        BGKO22_Two_RS_alg_round_type = BGKO22_TWO_RS_INFORMING_STATUS_2;
    } else if (BGKO22_Two_RS_alg_round_type ==
               BGKO22_TWO_RS_INFORMING_STATUS_2) {
        BGKO22_Two_RS_alg_round_type = BGKO22_TWO_RS_GENERATING_MARK;
    }
}

cMessage *BGKO22TwoRSAlg::process_message_queue()
{
    EV << "\t"
       << "BGKO22TwoRSAlg::process_message_queue()\n";
    previous_MIS_status = MIS_status;
    if (is_decided()) {
        EV << "\t\t"
           << "Status is " << MIS_status << " decided at round "
           << last_communication_round << "\n";
        return nullptr;
    }

    // Init the number of undecided neighbors to be the number of neighbors.
    // Only do this once.
    if (previous_round_alg_stage == BGKO22TwoRSStage::INITIAL_STAGE &&
        current_round_alg_stage == BGKO22TwoRSStage::BGKO22_TWO_RS_STAGE) {
        degree = node->all_neighbors.size();
    }

    cMessage *msg = nullptr;

    if (BGKO22_Two_RS_alg_round_type == BGKO22_TWO_RS_GENERATING_MARK) {
        msg = process_message_queue_for_generate_mark_round();
    } else if (BGKO22_Two_RS_alg_round_type ==
               BGKO22_TWO_RS_INFORMING_STATUS_1) {
        msg = process_message_queue_for_informing_status_1_round();
    } else if (BGKO22_Two_RS_alg_round_type ==
               BGKO22_TWO_RS_INFORMING_STATUS_2) {
        msg = process_message_queue_for_informing_status_2_round();
    }

    EV << "BGKO22TwoRSAlg::process_message_queue() -- all_remained_neighbors = "
          "[";
    for (int x : all_remained_neighbors)
        EV << x << ",";
    EV << "]\n";

    return msg;
}

cMessage *BGKO22TwoRSAlg::process_message_queue_for_generate_mark_round()
{
    EV << "BGKO22TwoRSAlg::process_message_queue_for_generate_mark_round()\n";
    for (cMessage *msg : message_queue) {
        LamMISMessage *Lam_MIS_message = dynamic_cast<LamMISMessage *>(msg);
        int neighbor_id = Lam_MIS_message->getSenderId();
        LamTwoRSNodeStatus BGKO22_Two_RS_neighbor_status =
            Lam_MIS_message->getTwoRSStatus();
        if (BGKO22_Two_RS_neighbor_status != LAM_TWO_RS_UNDECIDED)
            all_remained_neighbors.erase(neighbor_id);
        if (BGKO22_Two_RS_neighbor_status == LAM_TWO_RS_1_HOP) {
            if (BGKO22_Two_RS_status == LAM_TWO_RS_UNDECIDED) {
                BGKO22_Two_RS_status = LAM_TWO_RS_2_HOP;
                // parent = neighbor_id;
                two_rs_cluster_center_id =
                    Lam_MIS_message->getClusterCenterId();
            }
        }
    }

    if (BGKO22_Two_RS_status == LAM_TWO_RS_UNDECIDED) {
        marked = node->bernoulli(1.0 / (2 * degree));
        EV << "\tJoin a competition\n";
    } else if (BGKO22_Two_RS_status == LAM_TWO_RS_2_HOP) {
        marked = false;
        EV << "\tBecome 2-hop 2RS of cluster " << two_rs_cluster_center_id
           << "\n";
    }
    LamMISMessage *new_message = new LamMISMessage("LamMISMessage_2RS");
    new_message->setSenderId(node->id);
    new_message->setMarked(marked);
    new_message->setDegree(degree);
    new_message->setTwoRSStatus(BGKO22_Two_RS_status);
    new_message->setClusterCenterId(two_rs_cluster_center_id);
    need_to_send = all_remained_neighbors;
    EV << "\t\t"
       << "marked = " << marked << ", degree = " << degree << '\n';
    return new_message;
}

cMessage *BGKO22TwoRSAlg::process_message_queue_for_informing_status_1_round()
{
    EV << "BGKO22TwoRSAlg::process_message_queue_for_informing_status_1_round()"
          "\n";
    degree = message_queue.size();
    if (degree == 0) {
        EV << "\t\t"
           << "degree = 0 --> become 2RS cluster center\n";
        BGKO22_Two_RS_status = LAM_TWO_RS_CLUSTER_CENTER;
        return nullptr;
    } else {
        EV << "\t\t"
           << "degree = " << degree << ", marked = " << marked;
        if (marked)
            EV << " --> find higher priority message\n";
        else
            EV << "\n";
        for (cMessage *msg : message_queue) {
            LamMISMessage *Lam_MIS_message = dynamic_cast<LamMISMessage *>(msg);
            int neighbor_id = Lam_MIS_message->getSenderId();
            bool neighbor_marked = Lam_MIS_message->getMarked();
            int neighbor_degree = Lam_MIS_message->getDegree();
            LamTwoRSNodeStatus Lam_Two_RS_neighbor_status =
                Lam_MIS_message->getTwoRSStatus();
            EV << "\t\t\t"
               << "neighbor_id = " << neighbor_id
               << ", neighbor_marked = " << neighbor_marked
               << ", neighbor_degree = " << neighbor_degree
               << ", Lam_Two_RS_neighbor_status = "
               << Lam_Two_RS_neighbor_status << '\n';
            if (Lam_Two_RS_neighbor_status != LAM_TWO_RS_UNDECIDED) {
                all_remained_neighbors.erase(neighbor_id);
                continue;
            }
            if (!neighbor_marked)
                continue;
            if ((neighbor_degree > degree) ||
                (neighbor_degree == degree && neighbor_id > node->id)) {
                marked = false;
            }
        }
        if (!marked)
            return nullptr;
        EV << "\t"
           << "Join 2RS\n";
        BGKO22_Two_RS_status = LAM_TWO_RS_CLUSTER_CENTER;
        two_rs_cluster_center_id = node->id;
        LamMISMessage *new_message = new LamMISMessage("LamMISMessage_2RS");
        new_message->setSenderId(node->id);
        new_message->setTwoRSStatus(BGKO22_Two_RS_status);
        new_message->setClusterCenterId(two_rs_cluster_center_id);
        need_to_send = all_remained_neighbors;
        return new_message;
    }
    return nullptr;
}

cMessage *BGKO22TwoRSAlg::process_message_queue_for_informing_status_2_round()
{
    EV << "BGKO22TwoRSAlg::process_message_queue_for_informing_status_2_round()"
          "\n";
    for (cMessage *msg : message_queue) {
        LamMISMessage *Lam_MIS_message = dynamic_cast<LamMISMessage *>(msg);
        int neighbor_id = Lam_MIS_message->getSenderId();
        LamTwoRSNodeStatus BGKO22_Two_RS_neighbor_status =
            Lam_MIS_message->getTwoRSStatus();
        EV << "\t\t\t"
           << "neighbor_id = " << neighbor_id
           << ", BGKO22_Two_RS_neighbor_status = "
           << BGKO22_Two_RS_neighbor_status
           << ", BGKO22_neighbor_cluster_center_id = "
           << Lam_MIS_message->getClusterCenterId() << '\n';
        if (BGKO22_Two_RS_neighbor_status == LAM_TWO_RS_CLUSTER_CENTER) {
            if (BGKO22_Two_RS_status == LAM_TWO_RS_UNDECIDED) {
                BGKO22_Two_RS_status = LAM_TWO_RS_1_HOP;
                two_rs_cluster_center_id =
                    Lam_MIS_message->getClusterCenterId();
                all_remained_neighbors.erase(neighbor_id);
            }
        }
    }
    if (BGKO22_Two_RS_status == LAM_TWO_RS_1_HOP) {
        EV << "\tBecome 1-hop 2RS of cluster " << two_rs_cluster_center_id
           << "\n";
        LamMISMessage *new_message = new LamMISMessage("LamMISMessage_2RS");
        new_message->setSenderId(node->id);
        new_message->setTwoRSStatus(BGKO22_Two_RS_status);
        new_message->setClusterCenterId(two_rs_cluster_center_id);
        need_to_send = all_remained_neighbors;
        return new_message;
    }
    return nullptr;
}

bool BGKO22TwoRSAlg::is_selected()
{
    return (BGKO22_Two_RS_status == LAM_TWO_RS_CLUSTER_CENTER);
}

bool BGKO22TwoRSAlg::is_decided()
{
    return (BGKO22_Two_RS_status != LAM_TWO_RS_UNDECIDED);
}

void BGKO22TwoRSAlg::update_previous_status()
{
    BGKO22_Two_RS_previous_status = BGKO22_Two_RS_status;
}

void BGKO22TwoRSAlg::record_decided_round()
{ // only record node that is
  // cluster center and 1 hop
    if (BGKO22_Two_RS_previous_status == LAM_TWO_RS_UNDECIDED &&
        BGKO22_Two_RS_status == LAM_TWO_RS_CLUSTER_CENTER) {
        decided_round = current_round_id;
    } else if (BGKO22_Two_RS_previous_status == LAM_TWO_RS_UNDECIDED &&
               BGKO22_Two_RS_status == LAM_TWO_RS_1_HOP) {
        decided_round = current_round_id;
    }
}