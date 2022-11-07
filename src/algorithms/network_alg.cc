#include "algorithms/network_alg.h"
#include "messages/synchronized_message.h"

NetworkAlg::NetworkAlg(Network *network, const char *alg_name) {
    this->network = network;
    this->n_nodes = this->network->n_nodes;
    IAlg::init(n_nodes, alg_name);
}

void NetworkAlg::handle_message(cMessage *msg) {
    if (msg->isSelfMessage() && msg->getKind() == START_ROUND_MESSAGE) {
        if (current_round_id == max_num_rounds) {
            EV << "NetworkAlg::handle_message() -- FINISH at Round " << current_round_id << "\n";
        } else {
            EV << "NetworkAlg::handle_message() -- START_ROUND_MESSAGE -- Round " << current_round_id << "\n";
            send_synchronized_message();
            ++current_round_id;
            network->scheduleAt(current_round_id, new cMessage("Start Rounds", START_ROUND_MESSAGE));
        }
    }
    delete msg;
}
 
/**
 * @brief 
 * 
 */
void NetworkAlg::send_synchronized_message() {
    EV << "NetworkAlg::send_synchronized_message()\n";
    SynchronizedMessage *synchronized_message = new SynchronizedMessage("synchronized");
    synchronized_message->setSenderId(-1);
    synchronized_message->setRoundId(current_round_id);
    synchronized_message->setSynchronizedMessageType(SYNCHRONIZED_START_ROUND);
    EV << "root msg: " << synchronized_message->getKind() << "\n";
    for(cGate *gate : network->nodesWirelessIn) {
        cMessage *msg = synchronized_message->dup();
        network->sendDirect(msg, gate);
        //EV << "node " << gate->getOwner()->getName() << ' ' << msg->getKind() << '\n';
    }
    delete synchronized_message;
}
