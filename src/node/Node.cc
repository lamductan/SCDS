#include "node/Node.h"
#include "algorithms/alg_factory.h"

void Node::initialize()
{
    alg = AlgFactory::create_alg(this, DUMMY_ALG);
    networkWirelessIn = getParentModule()->getSubmodule("builder")->gate("wirelessIn");
}

void Node::handleMessage(cMessage *msg)
{
    EV << "Node::handleMessage(cMessage *msg)\n";
    if (msg->isSelfMessage()) {
        EV << "self message \n";
    } else {
        alg->handle_message(msg);
    }
    delete msg;
}

Node::~Node() {
    delete alg;
}