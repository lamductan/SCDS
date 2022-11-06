#include <cassert>
#include <omnetpp.h>

#include "messages/messages.h"
#include "algorithms/dummy_alg.h"

using namespace omnetpp;

DummyAlg::DummyAlg(Node *node, const char *alg_name) : IAlgNode(node, alg_name) {}

void DummyAlg::process_round() {
    EV << "\tprocess_round()\n";
    EV << "\tRandom number: " << node->bernoulli(0.5) << "\n";
}

DummyAlg::~DummyAlg() {}