#include <cassert>
#include <omnetpp.h>

#include "messages/messages.h"
#include "algorithms/dummy_alg.h"

using namespace omnetpp;

/* Network */
DummyAlgNetwork::DummyAlgNetwork(Network *network) {
    this->network = network;
}

void DummyAlgNetwork::start() {}

void DummyAlgNetwork::handle_message(cMessage *msg) {
    if (msg->isSelfMessage() && msg->getKind() == START_ROUND_MESSAGE) {
        EV << "DummyAlgNetwork::handle_message() -- START_ROUND_MESSAGE\n";
        network->send_synchrozied_message();
    } else {
        if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
            SynchronizedMessage *synchronized_message = dynamic_cast<SynchronizedMessage *>(msg);
            int sender_id = synchronized_message->getSenderId();
            int round_id = synchronized_message->getRoundId();
            assert(round_id == network->current_round_id);
            assert(synchronized_message->getSynchronizedMessageType() == SYNCHRONIZED_COMPLETE);
            EV << "Node " << sender_id << " completed round " << round_id << "\n";
        }
    }
}

DummyAlgNetwork::~DummyAlgNetwork() {}

/* Node */
DummyAlgNode::DummyAlgNode(Node *node) {
    this->node = node;
}

void DummyAlgNode::start() {}

void DummyAlgNode::handle_message(cMessage *msg) {
    EV << "DummyAlgNode::handle_message(msg)\n";
    EV << "(msg->getKind() == SYNCHRONIZED_MESSAGE) = " << msg->getKind() << " = " << SYNCHRONIZED_MESSAGE << "\n";
    if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
        SynchronizedMessage *synchronized_message = dynamic_cast<SynchronizedMessage *>(msg);
        int sender_id = synchronized_message->getSenderId();
        assert(sender_id == -1);
        int round_id = synchronized_message->getRoundId();
        EV << "Node " << node->par("id") << " (getId() = " << node->getId() << ") " 
            << "received synchronized message of round " << round_id << "\n";
    }
}

DummyAlgNode::~DummyAlgNode() {}