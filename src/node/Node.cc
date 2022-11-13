#include "node/Node.h"
#include "algorithms/alg_factory.h"

void Node::initialize() {
    id = par("id").intValue();
    cluster_id = id;
    color = -1;
    n_nodes = par("n_nodes");
    EV << "Node::initialize() -- " << "n_nodes = " << n_nodes << ' ' 
       << "par(\"algorithm\").stringValue()" << par("algorithm").stringValue() << '\n';
    alg = AlgFactory::create_alg(this, par("algorithm").stringValue(), 1);
    networkWirelessIn = getParentModule()->getSubmodule("net")->gate("wirelessIn");
}

void Node::connect(cGate *src, cGate *dest, int destid)
{
    src->connectTo(dest);
    all_neighbors.push_back(destid);
    all_neighbors_original.push_back(destid);
    neighbor_gates[destid] = src;
}

void Node::handleMessage(cMessage *msg) {
    alg->handle_message(msg);
}

bool Node::is_decided() {
    return alg->is_decided();
}

Node::~Node() {
    delete alg;
}