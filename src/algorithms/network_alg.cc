#include "algorithms/network_alg.h"
#include "messages/synchronized_message.h"

NetworkAlg::NetworkAlg(Network *network, const char *alg_name) {
    this->network = network;
    this->n_nodes = this->network->n_nodes;
    this->alg_name = alg_name;
    set_alg_type();
    set_max_num_rounds(n_nodes);
}

void NetworkAlg::set_alg_type() {
    if (strncmp(alg_name, "MIS", 3) == 0) {
        alg_type = MIS_ALG;
    } else if (strncmp(alg_name, "CDS", 3) == 0) {
        alg_type = CDS_ALG;
    } else {
        alg_type = DUMMY_ALG;
    }
    EV << "NetworkAlg::set_alg_type: " << "alg_name = " << alg_name << ", alg_type = " << alg_type
       << ", strncmp(alg_name, \"MIS\", 3) = " << strncmp(alg_name, "MIS", 3) 
       << "n_nodes = " << n_nodes << '\n';
}

void NetworkAlg::handle_message(cMessage *msg) {
    if (msg->isSelfMessage() && msg->getKind() == START_ROUND_MESSAGE) {
        if (network->check(false)) {
            EV << "Got all selected nodes. Finish here.\n";
            send_synchronized_message(SYNCHRONIZED_FINISH);
            delete msg;
            return;
        }
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
void NetworkAlg::send_synchronized_message(SynchronizedMessageType synchronized_message_type) {
    EV << "NetworkAlg::send_synchronized_message(), type = " << synchronized_message_type << '\n';
    SynchronizedMessage *synchronized_message = new SynchronizedMessage("synchronized");
    synchronized_message->setSenderId(-1);
    synchronized_message->setRoundId(current_round_id);
    synchronized_message->setSynchronizedMessageType(synchronized_message_type);
    for(cGate *gate : network->nodesWirelessIn) {
        cMessage *msg = synchronized_message->dup();
        network->sendDirect(msg, gate);
        //EV << "node " << gate->getOwner()->getName() << ' ' << msg->getKind() << '\n';
    }
    delete synchronized_message;
}
