#include <cassert>
#include <omnetpp.h>

#include "messages/messages.h"
#include "algorithms/dummy_alg.h"

using namespace omnetpp;

DummyAlg::DummyAlg(Node *node, int starting_round) : IAlgNode(node, starting_round) {}

cMessage * DummyAlg::process_message_queue() {
    if (current_round_id <= 2) {
        return IAlgNode::process_message_queue();
    }

    EV << "\tprocess_remained_rounds() " << current_round_id << "\n";
    EV << "\tRandom number: " << node->bernoulli(0.5) << "\n";
    return nullptr;
}

bool DummyAlg::is_decided() {
    return true;
}

bool DummyAlg::is_awake() {
    return true;
}

DummyAlg::~DummyAlg() {}