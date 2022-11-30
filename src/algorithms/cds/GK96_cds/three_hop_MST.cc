#include "algorithms/cds/GK96_cds/three_hop_MST.h"

ThreeHopMSTAlg::ThreeHopMSTAlg(Node *node, int starting_round)
{
    init(node, starting_round);
}

void ThreeHopMSTAlg::init(Node *node, int starting_round)
{
    IAlgNode::init(node, starting_round);
    fragment_core = { INF_EDGE_ID, id };
}

int ThreeHopMSTAlg::find_next_relay_node_id_on_path_to(int next_node_on_path_id)
{
    return nodes_on_path[next_node_on_path_id][1];
}

void ThreeHopMSTAlg::process_async_message_queue()
{
    if (MIS_status == IN_MIS) {
        return GHSMSTAlg::process_async_message_queue();
    }

    std::set<int> already_processed_neighbors;
    std::vector<GHSMSTMessage *> processed_messages;
    for (GHSMSTMessage *msg : async_message_queue) {
        int sender_id = msg->getSenderId();
        if (already_processed_neighbors.count(sender_id) > 0) continue;
        GHSMSTMessage *new_msg = msg->dup();
        new_msg->setSenderId(id);
        new_msg->setReceiverId(
            find_next_relay_node_id_on_path_to(msg->getNextNodeOnPathId()));
        new_msg->setMessageId(message_id++);
        need_to_send_message_queue.insert(new_msg);
        if (msg->getGHSMSTMessageType() == GHS_MST_MESSAGE_CONNECT) {
            CDS_status = IN_CDS;
        }
        processed_messages.push_back(msg);
    }

    for (GHSMSTMessage *msg : processed_messages) {
        async_message_queue.erase(msg);
        delete msg;
    }
}

bool ThreeHopMSTAlg::is_awake()
{
    if (CDS_status == NOT_IN_CDS) return false;
    return GHSMSTAlg::is_awake();
}