#include "algorithms/mis/Luby_mis/Luby_MIS.h"
#include "algorithms/mis/Luby_mis/Luby_MIS_message.h"

void LubyMISAlg::set_alg_type() { EV << "LubyMIS::set_alg_type()\n"; alg_type = MIS_ALG; }

LubyMISAlg::LubyMISAlg(Node *node, int starting_round) {
    init(node, starting_round);
}

void LubyMISAlg::stage_transition() {
    previous_round_alg_stage = current_round_alg_stage;
    current_round_alg_stage = LubyMISStage::MIS_STAGE;
    if (current_round_id < max_num_rounds && Luby_alg_round_type == LUBY_GENERATING_MARK) {
        Luby_alg_round_type = LUBY_PROCESSING_MARK;
    } else if (current_round_id < max_num_rounds && Luby_alg_round_type == LUBY_PROCESSING_MARK) {
        Luby_alg_round_type = LUBY_GENERATING_MARK;
    } else {
        current_round_alg_stage = LubyMISStage::END_STAGE;
    }
}

cMessage *LubyMISAlg::process_message_queue() {
    EV << "\t" << "LubyMISAlg::process_message_queue()\n";
    previous_status = status;
    if (is_decided()) {
        EV << "\t\t" << "Status is " << status << " decided at round " << last_communication_round << "\n";
        return nullptr;
    }

    // Init the number of undecided neighbors to be the number of neighbors.
    // Only do this once.
    if (previous_round_alg_stage == LubyMISStage::INITIAL_STAGE
            && current_round_alg_stage == LubyMISStage::MIS_STAGE) {
        degree = node->all_neighbors.size();
    }

    if (Luby_alg_round_type == LUBY_GENERATING_MARK) {
        return process_message_queue_for_generate_mark_round();
    } else {
        return process_message_queue_for_processing_mark_round();
    }
    return nullptr;
}

cMessage *LubyMISAlg::process_message_queue_for_generate_mark_round() {
    EV << "LubyMISAlg::process_message_queue_for_generate_mark_round()\n";
    for(cMessage *msg : message_queue) {
        LubyMISMessage *Luby_MIS_message = dynamic_cast<LubyMISMessage *>(msg);
        int neighbor_id = Luby_MIS_message->getSenderId();
        NodeStatus neighbor_status = Luby_MIS_message->getStatus();
        if (neighbor_status != UNDECIDED) need_to_send.erase(neighbor_id);
        if (neighbor_status == IN_MIS) {
            status = NOT_IN_MIS;
            return nullptr;
        }
    }
    
    marked = node->bernoulli(1.0/(2*degree));
    LubyMISMessage *new_message = new LubyMISMessage("LubyMIS");
    new_message->setSenderId(node->id);
    new_message->setMarked(marked);
    new_message->setDegree(degree);
    EV << "\t\t" << "marked = " << marked << ", degree = " << degree << '\n';
    return new_message;
}

cMessage *LubyMISAlg::process_message_queue_for_processing_mark_round() {
    EV << "LubyMISAlg::process_message_queue_for_processing_mark_round()\n";
    degree = message_queue.size();
    if (degree == 0) {
        EV << "\t\t" << "degree = 0 --> join MIS\n";
        status = IN_MIS;
        return nullptr;
    } else {
        if (!marked) return nullptr;
        EV << "\t\t" << "degree = " << degree << ", marked = 1 --> find higher priority message\n";
        need_to_send.clear();
        for(cMessage *msg : message_queue) {
            LubyMISMessage *Luby_MIS_message = dynamic_cast<LubyMISMessage *>(msg);
            bool neighbor_marked = Luby_MIS_message->getMarked();
            int neighbor_degree = Luby_MIS_message->getDegree();
            int neighbor_id = Luby_MIS_message->getSenderId();
            need_to_send.insert(neighbor_id);
            EV << "\t\t\t" << "neighbor_id = " << neighbor_id << ", neighbor_marked = " << neighbor_marked
                << ", neighbor_degree = " << neighbor_degree << '\n';
            if (!neighbor_marked) continue;
            if ((neighbor_degree > degree) || (neighbor_degree == degree && neighbor_id > node->id)) {
                marked = false;
                break;
            }
        }
        if (!marked) return nullptr;
        status = IN_MIS;
        LubyMISMessage *new_message = new LubyMISMessage("LubyMIS");
        new_message->setSenderId(node->id);
        new_message->setStatus(status);
        return new_message;
    }
    return nullptr;
}

bool LubyMISAlg::is_selected() {
    return (status == IN_MIS);
}