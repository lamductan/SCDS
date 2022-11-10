#include "algorithms/mis/Luby_mis/Luby_MIS.h"
#include "algorithms/mis/Luby_mis/Luby_MIS_message.h"

void LubyMISAlg::set_alg_type() { EV << "LubyMIS::set_alg_type()\n"; alg_type = MIS_ALG; }

LubyMISAlg::LubyMISAlg(Node *node, int starting_round) {
    init(node, starting_round);
}

void LubyMISAlg::stage_transition() {
    previous_round_alg_stage = current_round_alg_stage;
    if (current_round_id < max_num_rounds) {
        current_round_alg_stage = LubyMISStage::MIS_STAGE;
    } else {
        current_round_alg_stage = LubyMISStage::END_STAGE;
    }
}

bool LubyMISAlg::is_marked_round() {
    return ((current_round_id - starting_round) % 2 == 0);
}

void LubyMISAlg::record_last_communication_round() {
    if (previous_status == UNDECIDED && status != UNDECIDED) {
        last_communication_round = current_round_id - 1;
    }
}

cMessage *LubyMISAlg::process_message_queue() {
    EV << "\t" << "LubyMISAlg::process_message_queue()\n";
    record_last_communication_round();

    previous_status = status;

    if (is_decided()) {
        EV << "\t\t" << "Status is " << status << " decided at round " << last_communication_round << "\n";
        return nullptr;
    }

    if (current_round_alg_stage == LubyMISStage::INITIAL_STAGE) {
        return IAlgNode::process_message_queue();
    }

    // Right after neighbor discovery stage, init the number of undecided neighbors
    // to be the number of neighbors.
    // Only do this once.
    if (previous_round_alg_stage == LubyMISStage::INITIAL_STAGE
            && current_round_alg_stage == LubyMISStage::MIS_STAGE) {
        
        degree = node->all_neighbors.size();
    }

    // Check if this round is for marking or not
    // 1 round for marking then 1 round for processing, alternatively
    if (is_marked_round()) {
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

    } else {
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
    }
    return nullptr;
}

bool LubyMISAlg::is_selected() {
    return (status == IN_MIS);
}

bool LubyMISAlg::is_decided() {
    return (status != UNDECIDED);
}