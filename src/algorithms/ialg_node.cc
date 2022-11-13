#include "algorithms/ialg_node.h"
#include "messages/messages.h"

using namespace omnetpp;

void IAlgNode::init(Node *node, int starting_round) {
    this->starting_round = starting_round;
    this->node = node;
    this->n_nodes = node->n_nodes;
    set_alg_type();
    set_max_num_rounds(this->n_nodes);
    current_round_alg_stage = BaseAlgStage::INITIAL_STAGE;
    previous_round_alg_stage = BaseAlgStage::INITIAL_STAGE;
    need_to_send = std::unordered_set<int>(node->all_neighbors.begin(), node->all_neighbors.end());
}

IAlgNode::IAlgNode() {}

IAlgNode::IAlgNode(Node *node, int starting_round) {
    init(node, starting_round);
}

bool IAlgNode::is_selected() {
    return true;
}

bool IAlgNode::is_awake() {
    if (!is_decided()) return true;
    return current_round_id <= decided_round;
}

void IAlgNode::handle_message(cMessage *msg) {
    if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
        SynchronizedMessage *synchronized_message = dynamic_cast<SynchronizedMessage *>(msg);
        if (synchronized_message->getSynchronizedMessageType() == SYNCHRONIZED_START_ROUND) {
            start_round(msg);
            process_round();
        } else {
            clear_message_queue();
            delete msg;
        }
    } else {
        listen_new_message(msg);
    }
}

void IAlgNode::record_decided_round() {
    if (previous_status == UNDECIDED && status != UNDECIDED) {
        decided_round = current_round_id;
    }
}

void IAlgNode::start_round(cMessage *msg) {
    SynchronizedMessage *synchronized_message = dynamic_cast<SynchronizedMessage *>(msg);
    int sender_id = synchronized_message->getSenderId();
    assert(sender_id == -1);
    current_round_id = synchronized_message->getRoundId();
    //EV << "node" << node->id 
    //   << " received synchronized message of round " << current_round_id << "\n";
    delete msg;
}

void IAlgNode::process_round() {
    EV << "IAlgNode::process_round()\n";
    update_previous_status();
    stage_transition();
    EV << "n_awake_rounds = " << n_awake_rounds << '\n';
    if (!is_awake()) {
        EV << "not awake\n";
        return;
    }
    ++n_awake_rounds;
    EV << "Is awake: n_awake_rounds = " << n_awake_rounds << '\n';
    EV << "pre_process: message_queue.size() = " << message_queue.size() << '\n';
    cMessage *new_message = process_message_queue();
    clear_message_queue();
    EV << "post_process: message_queue.size() = " << message_queue.size() << '\n';
    if (new_message != nullptr) send_new_message(new_message);
    record_decided_round();
}

void IAlgNode::stage_transition() {}

void IAlgNode::clear_message_queue() {
    for(cMessage *msg : message_queue) delete msg;
    message_queue.clear();
}

cMessage *IAlgNode::process_message_queue() {
    return nullptr;
}

void IAlgNode::send_new_message(cMessage *msg, double delay) {
    EV << "node" << node->id << " : need_to_send = [";
    for(int x : need_to_send) EV << x << ",";
    EV << "]\n";
    last_communication_round = current_round_id;
    if (need_to_send.size() == 1 && *need_to_send.begin() == -1) {
        /*
        int n_neighbors = node->gateSize("port");
        for(int i = 0; i < n_neighbors; ++i) {
            node->sendDelayed(msg->dup(), delay, "port$o", i);
        }
        */
       for(int neighbor_id : node->all_neighbors) {
            node->sendDelayed(msg->dup(), delay, node->neighbor_gates[neighbor_id]);
        }
    } else {
        for(int neighbor_id : need_to_send) {
            node->sendDelayed(msg->dup(), delay, node->neighbor_gates[neighbor_id]);
        }
    }
    delete msg;
}

void IAlgNode::listen_new_message(cMessage *msg) {
    EV << "is_awake() = " << is_awake() << '\n';
    if (!is_awake() || is_decided()) delete msg;
    else {
        message_queue.push_back(msg);
        last_communication_round = current_round_id;
        EV << "is_awake() = " << is_awake() << ' ' << "message_queue.size() = " << message_queue.size() << '\n';
    }
}

bool IAlgNode::is_decided() {
    return (status != UNDECIDED);
}

void IAlgNode::update_previous_status() {
    previous_status = status;
}

void IAlgNode::call_handle_message(IAlgNode *alg, cMessage *msg) {
    alg->handle_message(msg);
    status = alg->status;
    if (is_decided() && alg->decided_round > -1)
        decided_round = alg->decided_round;
    if (alg->last_communication_round > -1)
        last_communication_round = alg->last_communication_round;
}

IAlgNode::~IAlgNode() {
    clear_message_queue();
}