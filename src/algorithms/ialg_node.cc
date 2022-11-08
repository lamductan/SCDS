#include "algorithms/ialg_node.h"
#include "messages/messages.h"

using namespace omnetpp;

void IAlgNode::init(Node *node) {
    this->node = node;
    this->n_nodes = node->n_nodes;
    set_alg_type();
    set_max_num_rounds(this->n_nodes);
    current_round_alg_stage = BaseAlgStage::INITIAL_STAGE;
    previous_round_alg_stage = BaseAlgStage::INITIAL_STAGE;
}

IAlgNode::IAlgNode() {}

IAlgNode::IAlgNode(Node *node) {
    init(node);
}

bool IAlgNode::is_selected() {
    return true;
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
    stage_transition();
    cMessage *new_message = process_message_queue();
    clear_message_queue();
    if (new_message != nullptr) send_new_message(new_message);
}

void IAlgNode::stage_transition() {
    previous_round_alg_stage = current_round_alg_stage;
    if (current_round_alg_stage == BaseAlgStage::INITIAL_STAGE) {
        current_round_alg_stage = BaseAlgStage::NEIGHBOR_DISCOVERY_STAGE;
    } else if (current_round_id <= LAST_NEIGHBOR_DISCOVERY_ROUND) {
        current_round_alg_stage = BaseAlgStage::NEIGHBOR_DISCOVERY_STAGE;
    } else {
        current_round_alg_stage = BaseAlgStage::END_STAGE;
    }
}

void IAlgNode::clear_message_queue() {
    for(cMessage *msg : message_queue) delete msg;
    message_queue.clear();
}

cMessage * IAlgNode::process_message_queue() {
    EV << "\t" << "IAlgNode::process_message_queue()\n";
    //EV << "\t\talg_stage = " << alg_stage << ", " << "current_round_id = " << current_round_id << "\n";
    if (current_round_alg_stage == BaseAlgStage::NEIGHBOR_DISCOVERY_STAGE && current_round_id == 1) {
        NeighborDiscoveryMessage *neighbor_discovery_message = new NeighborDiscoveryMessage("neighbor_discovery");
        neighbor_discovery_message->setSenderId(node->id);
        return neighbor_discovery_message;
    } else if (current_round_alg_stage == BaseAlgStage::NEIGHBOR_DISCOVERY_STAGE 
            && current_round_id <= LAST_NEIGHBOR_DISCOVERY_ROUND) {
        //EV << "\t\t" << "message_queue.size() = " << message_queue.size() << '\n';
        for(cMessage *msg : message_queue) {
            NeighborDiscoveryMessage *neighbor_discovery_message = dynamic_cast<NeighborDiscoveryMessage *>(msg);
            int neighbor_id = neighbor_discovery_message->getSenderId();
            cGate *neighbor_gate = neighbor_discovery_message->getArrivalGate();
            all_neighbors.push_back(neighbor_id);
            all_neighbors_set.insert(neighbor_id);
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