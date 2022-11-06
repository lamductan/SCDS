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
            n_completed_round_nodes = 0;
        }
    } else {
        if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
            SynchronizedMessage *synchronized_message = dynamic_cast<SynchronizedMessage *>(msg);
            int sender_id = synchronized_message->getSenderId();
            int round_id = synchronized_message->getRoundId();
            assert(round_id == network->current_round_id);
            assert(synchronized_message->getSynchronizedMessageType() == SYNCHRONIZED_COMPLETE);
            EV << "Node " << sender_id << " completed round " << round_id << "\n";
            ++n_completed_round_nodes;
            if (n_completed_round_nodes == n_nodes) {
                network->scheduleAt(1 + current_round_id, new cMessage("Start Rounds", START_ROUND_MESSAGE));
            }
        }
    }
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
