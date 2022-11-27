#include "algorithms/cds/cds_simple/simple_mis_to_cds.h"
#include "algorithms/cds/cds_simple/simple_cds_message.h"
#include "utils/utils.h"

void SimpleMISToCDSAlg::set_alg_type()
{
    EV << "SimpleMISToCDSAlg::set_alg_type()\n";
    alg_type = CDS_ALG;
}

SimpleMISToCDSAlg::SimpleMISToCDSAlg(Node *node, int starting_round)
{
    init(node, starting_round);
    max_num_rounds = 6;
}

bool SimpleMISToCDSAlg::is_selected() { return (CDS_status == IN_CDS); }

bool SimpleMISToCDSAlg::is_awake() { return true; }

cMessage *SimpleMISToCDSAlg::process_message_queue()
{
    int round_id = current_round_id - starting_round;
    // EV << "SimpleMISToCDSAlg::process_message_queue() -- message_queue.size()
    // = "
    //    << message_queue.size() << '\n';
    // for(cMessage *msg : message_queue) {
    //     EV << dynamic_cast<SimpleCDSMessage *>(msg)->to_string(1) << '\n';
    // }

    switch (round_id) {
    case 0:
        return process_message_queue_exchange_MIS_status();
        break;
    case 1:
        return process_message_queue_exchange_one_hop_MIS_neighbors();
        break;
    case 2:
        return process_message_queue_exchange_two_hop_MIS_neighbors();
        break;
    case 3:
        return process_message_queue_MIS_nodes_inform_one_hop_neighbors();
        break;
    case 4:
        return process_message_queue_one_hop_MIS_neighbors_inform_two_hop_MIS_neighbors();
        break;
    case 5:
        return process_message_queue_two_hop_MIS_neighbors();
        break;
    default:
        break;
    }
    return nullptr;
}

cMessage *SimpleMISToCDSAlg::process_message_queue_exchange_MIS_status()
{
    EV << "SimpleMISToCDSAlg::process_message_queue_exchange_MIS_status() -- "
          "message_queue.size() = "
       << message_queue.size() << '\n';
    if (MIS_status == NOT_IN_MIS)
        return nullptr;
    CDS_status = IN_CDS;
    SimpleCDSMessage *new_message = new SimpleCDSMessage("exchange_MIS_status");
    new_message->setSenderId(id);
    return new_message;
}

cMessage *
SimpleMISToCDSAlg::process_message_queue_exchange_one_hop_MIS_neighbors()
{
    EV << "SimpleMISToCDSAlg::process_message_queue_exchange_one_hop_MIS_node()"
          " -- message_queue.size() = "
       << message_queue.size() << '\n';
    if (message_queue.size() == 0)
        return nullptr;
    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        int sender_id = simple_CDS_message->getSenderId();
        MIS_in_one_hop_neighbor[sender_id] = { sender_id, id, -1 };
    }
    if (MIS_in_one_hop_neighbor.empty())
        return nullptr;
    SimpleCDSMessage *new_message =
        new SimpleCDSMessage("exchange_one_hop_MIS_neighbors");
    new_message->setSenderId(id);
    new_message->setMISInfo(MIS_in_one_hop_neighbor);
    return new_message;
}

cMessage *
SimpleMISToCDSAlg::process_message_queue_exchange_two_hop_MIS_neighbors()
{
    EV << "SimpleMISToCDSAlg::process_message_queue_exchange_two_hop_MIS_"
          "neighbors() -- message_queue.size() = "
       << message_queue.size() << '\n';
    if (message_queue.size() == 0)
        return nullptr;
    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        int sender_id = simple_CDS_message->getSenderId();
        std::map<int, std::array<int, 3>> candidate_two_hop_MIS_nodes =
            simple_CDS_message->getMISInfo();
        for (auto it : candidate_two_hop_MIS_nodes) {
            int candidate_two_hop_MIS_node_id = it.first;
            if (candidate_two_hop_MIS_node_id == id)
                continue;
            if (MIS_in_one_hop_neighbor.count(candidate_two_hop_MIS_node_id) >
                0)
                continue;
            if (MIS_in_two_hop_neighbor.count(candidate_two_hop_MIS_node_id) >
                0)
                continue;
            MIS_in_two_hop_neighbor[candidate_two_hop_MIS_node_id] = {
                candidate_two_hop_MIS_node_id, it.second[1], id
            };
        }
    }
    if (MIS_in_two_hop_neighbor.empty())
        return nullptr;
    SimpleCDSMessage *new_message =
        new SimpleCDSMessage("exchange_two_hop_MIS_neighbors");
    new_message->setSenderId(id);
    new_message->setMISInfo(MIS_in_two_hop_neighbor);
    return new_message;
}

cMessage *
SimpleMISToCDSAlg::process_message_queue_MIS_nodes_inform_one_hop_neighbors()
{
    EV << "SimpleMISToCDSAlg::process_message_queue_MIS_nodes_inform_one_hop_"
          "neighbors() -- message_queue.size() = "
       << message_queue.size() << '\n';
    if (MIS_status == NOT_IN_MIS)
        return nullptr;
    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        int sender_id = simple_CDS_message->getSenderId();
        std::map<int, std::array<int, 3>> candidate_three_hop_MIS_nodes =
            simple_CDS_message->getMISInfo();
        for (auto it : candidate_three_hop_MIS_nodes) {
            int candidate_three_hop_MIS_node_id = it.first;
            if (candidate_three_hop_MIS_node_id == id)
                continue;
            if (MIS_in_one_hop_neighbor.count(candidate_three_hop_MIS_node_id) >
                0)
                continue;
            if (MIS_in_two_hop_neighbor.count(candidate_three_hop_MIS_node_id) >
                0)
                continue;
            if (MIS_in_three_hop_neighbor.count(
                    candidate_three_hop_MIS_node_id) > 0)
                continue;
            MIS_in_three_hop_neighbor[candidate_three_hop_MIS_node_id] =
                it.second;
        }
    }

    MIS_in_two_three_hop_neighbor = combine_map<int, std::array<int, 3>>(
        MIS_in_three_hop_neighbor, MIS_in_two_hop_neighbor);
    std::set<int> cds_nodes;
    for (auto it : MIS_in_two_three_hop_neighbor) {
        for (int i = 1; i < 3; ++i) {
            int cds_node = it.second[i];
            if (cds_node == -1)
                break;
            cds_nodes.insert(cds_node);
        }
    }
    cds_nodes.erase(id);
    SimpleCDSMessage *new_message =
        new SimpleCDSMessage("nodes_inform_one_hop_neighbors");
    new_message->setSenderId(id);
    new_message->setCDSNodes(cds_nodes);
    return new_message;
}

cMessage *SimpleMISToCDSAlg::
    process_message_queue_one_hop_MIS_neighbors_inform_two_hop_MIS_neighbors()
{
    EV << "SimpleMISToCDSAlg::process_message_queue_one_hop_MIS_neighbors_"
          "inform_two_hop_MIS_neighbors() -- message_queue.size() = "
       << message_queue.size() << '\n';
    if (MIS_status == IN_MIS)
        return nullptr;
    std::set<int> cds_nodes;
    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        int sender_id = simple_CDS_message->getSenderId();
        std::set<int> received_cds_nodes = simple_CDS_message->getCDSNodes();
        if (CDS_status == NOT_IN_CDS && received_cds_nodes.count(id) > 0)
            CDS_status = IN_CDS;
        cds_nodes = combine_set<int>(cds_nodes, received_cds_nodes);
    }

    SimpleCDSMessage *new_message =
        new SimpleCDSMessage("nodes_inform_two_hop_neighbors");
    new_message->setSenderId(id);
    new_message->setCDSNodes(cds_nodes);
    return new_message;
}

cMessage *SimpleMISToCDSAlg::process_message_queue_two_hop_MIS_neighbors()
{
    EV << "SimpleMISToCDSAlg::process_message_queue_two_hop_MIS_neighbors() -- "
          "message_queue.size() = "
       << message_queue.size() << '\n';
    if (MIS_status == IN_MIS)
        return nullptr;
    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        int sender_id = simple_CDS_message->getSenderId();
        std::set<int> received_cds_nodes = simple_CDS_message->getCDSNodes();
        if (CDS_status == NOT_IN_CDS && received_cds_nodes.count(id) > 0)
            CDS_status = IN_CDS;
    }

    return nullptr;
}