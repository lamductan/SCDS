#include "node/Node.h"
#include "algorithms/alg_factory.h"

void Node::initialize()
{
    id = par("id").intValue();
    n_nodes = par("n_nodes");
    alg = AlgFactory::create_alg(this, par("algorithm").stringValue());
    networkWirelessIn = getParentModule()->getSubmodule("builder")->gate("wirelessIn");
}

void Node::handleMessage(cMessage *msg)
{
    EV << "Node::handleMessage(cMessage *msg)" << msg->getKind() << "\n";
    alg->handle_message(msg);
}

Node::~Node() {
    delete alg;
}