#include <cassert>
#include <omnetpp.h>

#include "messages/messages.h"
#include "algorithms/dummy_alg.h"

using namespace omnetpp;

DummyAlg::DummyAlg(Node *node) : IAlgNode(node) {}

cMessage * DummyAlg::process_message_queue() {
    if (current_round_id <= 2) {
        return IAlgNode::process_message_queue();
    }

    EV << "\tprocess_remained_rounds() " << current_round_id << "\n";
    EV << "\tRandom number: " << node->bernoulli(0.5) << "\n";
    return nullptr;
}

DummyAlg::~DummyAlg() {}