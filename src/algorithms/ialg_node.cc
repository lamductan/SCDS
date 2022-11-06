#include "algorithms/ialg_node.h"
#include "messages/synchronized_message.h"

using namespace omnetpp;

IAlgNode::IAlgNode(Node *node, const char *alg_name) {
    this->node = node;
    this->n_nodes = node->n_nodes;
    init(this->n_nodes, alg_name);
}

void IAlgNode::handle_message(cMessage *msg) {
    EV << "DummyAlgNode::handle_message(msg)\n";
    if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
        start_round(msg);
        process_round();
        end_round();
    }
}

void IAlgNode::start_round(cMessage *msg) {
    SynchronizedMessage *synchronized_message = dynamic_cast<SynchronizedMessage *>(msg);
    int sender_id = synchronized_message->getSenderId();
    assert(sender_id == -1);
    current_round_id = synchronized_message->getRoundId();
    EV << "node" << node->id 
       << " received synchronized message of round " << current_round_id << "\n";
}

void IAlgNode::end_round() {
    EV << "IAlgNode::end_round()\n";
    SynchronizedMessage *synchronized_message = new SynchronizedMessage("synchronized");
    synchronized_message->setSenderId(node->id);
    synchronized_message->setRoundId(current_round_id);
    synchronized_message->setSynchronizedMessageType(SYNCHRONIZED_COMPLETE);
    node->sendDirect(synchronized_message, node->networkWirelessIn);
    EV << "Node " << node->id << " completed round " << current_round_id << "\n";
}