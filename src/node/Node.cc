#include "node/Node.h"
#include "algorithms/alg_factory.h"

void Node::initialize() {
    id = par("id").intValue();
    n_nodes = par("n_nodes");
    EV << "Node::initialize() -- " << "n_nodes = " << n_nodes << ' ' 
       << "par(\"algorithm\").stringValue()" << par("algorithm").stringValue() << '\n';
    alg = AlgFactory::create_alg(this, par("algorithm").stringValue(), LAST_NEIGHBOR_DISCOVERY_ROUND + 1);
    networkWirelessIn = getParentModule()->getSubmodule("net")->gate("wirelessIn");
}

void Node::handleMessage(cMessage *msg) {
    alg->handle_message(msg);
}

Node::~Node() {
    delete alg;
}