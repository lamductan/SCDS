#include "algorithms/ialg_node.h"
#include "messages/messages.h"

using namespace omnetpp;

IAlgNode::IAlgNode(Node *node, const char *alg_name) {
    this->node = node;
    this->n_nodes = node->n_nodes;
    init(this->n_nodes, alg_name);
    alg_stage = INITIAL_STAGE;
    selected = true; // Must be override to false by derived classes
}

bool IAlgNode::is_selected() {
    return selected;
}

bool IAlgNode::is_awake() {
    return true;
}

void IAlgNode::handle_message(cMessage *msg) {
    EV << "DummyAlgNode::handle_message(msg)\n";
    if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
        start_round(msg);
        process_round();
    } else {
        listen_new_message(msg);
    }
}

void IAlgNode::start_round(cMessage *msg) {
    SynchronizedMessage *synchronized_message = dynamic_cast<SynchronizedMessage *>(msg);
    int sender_id = synchronized_message->getSenderId();
    assert(sender_id == -1);
    current_round_id = synchronized_message->getRoundId();
    EV << "node" << node->id 
       << " received synchronized message of round " << current_round_id << "\n";
    delete msg;
}

void IAlgNode::process_round() {
    if (!is_awake()) return;
    ++n_awake_rounds;
    alg_stage = stage_transition();
    cMessage *new_message = process_message_queue();
    clear_message_queue();
    if (new_message != nullptr) send_new_message(new_message);
}

IAlgNode::AlgStage IAlgNode::stage_transition() {
    if (alg_stage == INITIAL_STAGE) {
        return NEIGHBOR_DISCOVERY_STAGE;
    } else if (alg_stage == NEIGHBOR_DISCOVERY_STAGE && current_round_id <= LAST_NEIGHBOR_DISCOVERY_ROUND) {
        return NEIGHBOR_DISCOVERY_STAGE;
    } else if (alg_stage == NEIGHBOR_DISCOVERY_STAGE && current_round_id > LAST_NEIGHBOR_DISCOVERY_ROUND) {
        return MIS_STAGE;
    }
    return FINAL_STAGE;
}

void IAlgNode::clear_message_queue() {
    for(cMessage *msg : message_queue) delete msg;
    message_queue.clear();
}

cMessage * IAlgNode::process_message_queue() {
    EV << "\tprocess_message_queue()\n";
    //EV << "\t\talg_stage = " << alg_stage << ", " << "current_round_id = " << current_round_id << "\n";
    if (alg_stage == NEIGHBOR_DISCOVERY_STAGE && current_round_id == 1) {
        NeighborDiscoveryMessage *neighbor_discovery_message = new NeighborDiscoveryMessage("neighbor_discovery");
        neighbor_discovery_message->setSenderId(node->id);
        return neighbor_discovery_message;
    } else if (alg_stage == NEIGHBOR_DISCOVERY_STAGE && current_round_id <= LAST_NEIGHBOR_DISCOVERY_ROUND) {
        //EV << "\t\t" << "message_queue.size() = " << message_queue.size() << '\n';
        for(cMessage *msg : message_queue) {
            NeighborDiscoveryMessage *neighbor_discovery_message = dynamic_cast<NeighborDiscoveryMessage *>(msg);
            int neighbor_id = neighbor_discovery_message->getSenderId();
            cGate *neighbor_gate = neighbor_discovery_message->getArrivalGate();
            all_neighbors.push_back(neighbor_id);
            neighbor_gates[neighbor_id] = neighbor_gate;
            //EV << "\t\t" << "neighbor " << neighbor_id << ": gate " << neighbor_gate->getName() << "\n";
        }
        return nullptr;
    }
    return nullptr;
}

void IAlgNode::send_new_message(cMessage *msg) {
    int n_neighbors = node->gateSize("port");
    EV << "\tsend_new_message " << current_round_id << '\n';
    for(int i = 0; i < n_neighbors; ++i) {
        node->sendDelayed(msg->dup(), 0.5, "port$o", i);
    }
    delete msg;
}

void IAlgNode::listen_new_message(cMessage *msg) {
    if (!is_awake()) delete msg;
    message_queue.push_back(msg);
}