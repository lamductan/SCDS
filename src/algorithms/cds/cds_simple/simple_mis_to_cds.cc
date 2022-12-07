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
    max_num_rounds = 8;
}

bool SimpleMISToCDSAlg::is_selected() { return (CDS_status == IN_CDS); }

bool SimpleMISToCDSAlg::is_awake() { return true; }

cMessage *SimpleMISToCDSAlg::process_message_queue()
{
    int round_id = current_round_id - starting_round;
    awake_round_map[current_round_id] = true;
    // EV << "SimpleMISToCDSAlg::process_message_queue() -- message_queue.size()
    // = "
    //    << message_queue.size() << '\n';
    // for(cMessage *msg : message_queue) {
    //     EV << dynamic_cast<SimpleCDSMessage *>(msg)->to_string(1) << '\n';
    // }

    switch (round_id) {
    case 0:
        return process_message_queue_exchange_MIS_status();
    case 1:
        return process_message_queue_exchange_one_hop_MIS_neighbors();
    case 2:
        return process_message_queue_exchange_two_hop_MIS_neighbors();
    case 3:
        return process_message_queue_MIS_nodes_inform_one_hop_neighbors();
    case 4:
        return process_message_queue_one_hop_MIS_neighbors_inform_two_hop_MIS_neighbors();
    case 5:
        return process_message_queue_two_hop_MIS_neighbors_inform_MIS_nodes();
    case 6:
        return process_message_queue_MIS_nodes();
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
    if (MIS_status != IN_MIS) return nullptr;
    // CDS_status = IN_CDS;
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
    if (message_queue.size() == 0) return nullptr;
    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        if (MIS_status == UNDECIDED) MIS_status = NOT_IN_MIS;
        int sender_id = simple_CDS_message->getSenderId();
        MIS_in_one_hop_neighbor[sender_id] = { sender_id, id, -1 };
        nodes_on_path[sender_id] = { id, sender_id, -1, -1 };
    }
    if (MIS_in_one_hop_neighbor.empty()) return nullptr;
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
    if (message_queue.size() == 0) return nullptr;
    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        int sender_id = simple_CDS_message->getSenderId();
        std::map<int, std::array<int, 3>> candidate_two_hop_MIS_nodes =
            simple_CDS_message->getMISInfo();
        for (auto it : candidate_two_hop_MIS_nodes) {
            int candidate_two_hop_MIS_node_id = it.first;
            if (candidate_two_hop_MIS_node_id == id) continue;
            if (MIS_in_one_hop_neighbor.count(candidate_two_hop_MIS_node_id) >
                0)
                continue;
            if (MIS_in_two_hop_neighbor.count(candidate_two_hop_MIS_node_id) >
                0)
                continue;
            int intermedidate_node = it.second[1];
            MIS_in_two_hop_neighbor[candidate_two_hop_MIS_node_id] = {
                candidate_two_hop_MIS_node_id, intermedidate_node, id
            };
            if (id > candidate_two_hop_MIS_node_id) continue;

            nodes_on_path[candidate_two_hop_MIS_node_id] = {
                id, intermedidate_node, candidate_two_hop_MIS_node_id, -1
            };

            if (MIS_status == IN_MIS)
                nodes_on_directed_paths[{ 1, id,
                                          candidate_two_hop_MIS_node_id }] = {
                    id, intermedidate_node, candidate_two_hop_MIS_node_id, -1
                };
        }
    }
    if (MIS_in_two_hop_neighbor.empty()) return nullptr;
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
    if (MIS_status == NOT_IN_MIS) return nullptr;
    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        int sender_id = simple_CDS_message->getSenderId();
        std::map<int, std::array<int, 3>> candidate_three_hop_MIS_nodes =
            simple_CDS_message->getMISInfo();
        for (auto it : candidate_three_hop_MIS_nodes) {
            int candidate_three_hop_MIS_node_id = it.first;
            if (candidate_three_hop_MIS_node_id == id) continue;
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
            if (id > candidate_three_hop_MIS_node_id) continue;

            nodes_on_path[candidate_three_hop_MIS_node_id] = {
                id, it.second[2], it.second[1], candidate_three_hop_MIS_node_id
            };

            if (MIS_status == IN_MIS)
                nodes_on_directed_paths[{ 2, id,
                                          candidate_three_hop_MIS_node_id }] = {
                    id, it.second[2], it.second[1],
                    candidate_three_hop_MIS_node_id
                };
        }
    }

    MIS_in_two_three_hop_neighbor = combine_map<int, std::array<int, 3>>(
        MIS_in_three_hop_neighbor, MIS_in_two_hop_neighbor);
    std::set<int> cds_nodes;
    for (auto it : MIS_in_two_three_hop_neighbor) {
        for (int i = 1; i < 3; ++i) {
            int cds_node = it.second[i];
            if (cds_node == -1) break;
            cds_nodes.insert(cds_node);
        }
    }
    cds_nodes.erase(id);
    SimpleCDSMessage *new_message =
        new SimpleCDSMessage("nodes_inform_one_hop_neighbors");
    new_message->setSenderId(id);
    new_message->setCDSNodes(cds_nodes);
    new_message->setNodesOnDirectedPaths(nodes_on_directed_paths);
    return new_message;
}

cMessage *SimpleMISToCDSAlg::
    process_message_queue_one_hop_MIS_neighbors_inform_two_hop_MIS_neighbors()
{
    EV << "SimpleMISToCDSAlg::process_message_queue_one_hop_MIS_neighbors_"
          "inform_two_hop_MIS_neighbors() -- message_queue.size() = "
       << message_queue.size() << '\n';
    if (MIS_status == IN_MIS) return nullptr;
    std::set<int> cds_nodes;
    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        int sender_id = simple_CDS_message->getSenderId();
        std::set<int> received_cds_nodes = simple_CDS_message->getCDSNodes();
        if (CDS_status == NOT_IN_CDS && received_cds_nodes.count(id) > 0)
            CDS_status = IN_CDS;
        cds_nodes = combine_set<int>(cds_nodes, received_cds_nodes);
        nodes_on_directed_paths =
            combine_map<std::tuple<int, int, int>, std::array<int, 4>>(
                nodes_on_directed_paths,
                get_relevant_paths(
                    simple_CDS_message->getNodesOnDirectedPaths()));
    }

    nodes_on_directed_paths =
        combine_map<std::tuple<int, int, int>, std::array<int, 4>>(
            nodes_on_directed_paths,
            reverse_directed_path(nodes_on_directed_paths));
    pos_of_relay_on_directed_paths =
        extract_pos_of_relay_on_directed_paths(nodes_on_directed_paths);
    nodes_on_path = compute_nodes_on_path(nodes_on_directed_paths);

    SimpleCDSMessage *new_message =
        new SimpleCDSMessage("nodes_inform_two_hop_neighbors");
    new_message->setSenderId(id);
    new_message->setCDSNodes(cds_nodes);
    new_message->setNodesOnDirectedPaths(nodes_on_directed_paths);
    return new_message;
}

cMessage *SimpleMISToCDSAlg::
    process_message_queue_two_hop_MIS_neighbors_inform_MIS_nodes()
{
    EV << "SimpleMISToCDSAlg::process_message_queue_two_hop_MIS_neighbors_"
          "inform_MIS_nodes() -- message_queue.size() = "
       << message_queue.size() << '\n';
    if (MIS_status == IN_MIS) return nullptr;

    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        int sender_id = simple_CDS_message->getSenderId();
        std::set<int> received_cds_nodes = simple_CDS_message->getCDSNodes();
        if (CDS_status == NOT_IN_CDS && received_cds_nodes.count(id) > 0)
            CDS_status = IN_CDS;
        nodes_on_directed_paths =
            combine_map<std::tuple<int, int, int>, std::array<int, 4>>(
                nodes_on_directed_paths,
                get_relevant_paths(
                    simple_CDS_message->getNodesOnDirectedPaths()));
    }

    nodes_on_directed_paths =
        combine_map<std::tuple<int, int, int>, std::array<int, 4>>(
            nodes_on_directed_paths,
            reverse_directed_path(nodes_on_directed_paths));
    pos_of_relay_on_directed_paths =
        extract_pos_of_relay_on_directed_paths(nodes_on_directed_paths);
    nodes_on_path = compute_nodes_on_path(nodes_on_directed_paths);

    SimpleCDSMessage *new_message =
        new SimpleCDSMessage("nodes_inform_two_hop_neighbors");
    new_message->setSenderId(id);
    new_message->setNodesOnDirectedPaths(nodes_on_directed_paths);
    return new_message;
}

cMessage *SimpleMISToCDSAlg::process_message_queue_MIS_nodes()
{
    EV << "SimpleMISToCDSAlg::process_message_queue_MIS_nodes -- "
          "message_queue.size() = "
       << message_queue.size() << '\n';

    // Only do local computation in this round
    --n_awake_rounds;
    awake_round_map[current_round_id] = false;

    if (MIS_status == NOT_IN_MIS) return nullptr;
    // TODO: computer path from greater id to smaller id

    std::map<std::tuple<int, int, int>, std::array<int, 4>>
        all_received_nodes_on_directed_paths;
    for (cMessage *msg : message_queue) {
        SimpleCDSMessage *simple_CDS_message =
            dynamic_cast<SimpleCDSMessage *>(msg);
        int sender_id = simple_CDS_message->getSenderId();
        std::set<int> received_cds_nodes = simple_CDS_message->getCDSNodes();
        if (CDS_status == NOT_IN_CDS && received_cds_nodes.count(id) > 0)
            CDS_status = IN_CDS;
        std::map<std::tuple<int, int, int>, std::array<int, 4>>
            received_nodes_on_directed_paths =
                simple_CDS_message->getNodesOnDirectedPaths();
        all_received_nodes_on_directed_paths =
            combine_map<std::tuple<int, int, int>, std::array<int, 4>>(
                all_received_nodes_on_directed_paths,
                received_nodes_on_directed_paths);
    }
    for (auto it : all_received_nodes_on_directed_paths) {
        std::tuple<int, int, int> edge = it.first;
        if (std::get<1>(edge) != id && std::get<2>(edge) != id) continue;
        std::array<int, 4> path = it.second;
        if (nodes_on_directed_paths.count(edge)) continue;
        nodes_on_directed_paths[edge] = path;
    }
    nodes_on_path = compute_nodes_on_path(nodes_on_directed_paths);
    if (CDS_status == TENTATIVE_IN_CDS) CDS_status = IN_CDS;
    return nullptr;
}

std::map<std::tuple<int, int, int>, std::array<int, 4>>
SimpleMISToCDSAlg::get_relevant_paths(
    const std::map<std::tuple<int, int, int>, std::array<int, 4>>
        &nodes_on_directed_paths) const
{
    std::map<std::tuple<int, int, int>, std::array<int, 4>> relevant_paths;
    for (auto it : nodes_on_directed_paths) {
        std::tuple<int, int, int> edge = it.first;
        std::array<int, 4> path = it.second;
        if (path[1] != id && path[2] != id) continue;
        relevant_paths[edge] = path;
    }
    return relevant_paths;
}

std::map<std::tuple<int, int, int>, std::array<int, 4>>
SimpleMISToCDSAlg::reverse_directed_path(
    const std::map<std::tuple<int, int, int>, std::array<int, 4>>
        &nodes_on_directed_paths)
{
    std::map<std::tuple<int, int, int>, std::array<int, 4>>
        reverse_nodes_on_directed_paths;
    for (auto it : nodes_on_directed_paths) {
        std::tuple<int, int, int> edge = it.first;
        std::array<int, 4> path = it.second;
        reverse_nodes_on_directed_paths[reverse_edge(edge)] =
            reverse_path(path);
    }
    return reverse_nodes_on_directed_paths;
}

std::map<int, std::array<int, 4>> SimpleMISToCDSAlg::compute_nodes_on_path(
    const std::map<std::tuple<int, int, int>, std::array<int, 4>>
        &nodes_on_directed_paths)
{
    std::map<int, std::array<int, 4>> nodes_on_path_1;
    for (auto it : nodes_on_directed_paths) {
        std::tuple<int, int, int> edge = it.first;
        std::array<int, 4> path = it.second;
        int receiver_id = std::get<2>(edge);
        for (int i = 0; i <= 2; ++i) {
            if (path[i] != id) continue;
            nodes_on_path_1[receiver_id][1] = path[i + 1];
            break;
        }
    }
    return nodes_on_path_1;
}

std::tuple<int, int, int>
SimpleMISToCDSAlg::reverse_edge(const std::tuple<int, int, int> &t)
{
    return { std::get<0>(t), std::get<2>(t), std::get<1>(t) };
}

std::array<int, 4>
SimpleMISToCDSAlg::reverse_path(const std::array<int, 4> &arr)
{
    if (arr[3] == -1) return { arr[2], arr[1], arr[0], -1 };
    return { arr[3], arr[2], arr[1], arr[0] };
}

std::map<std::tuple<int, int, int>, int>
SimpleMISToCDSAlg::extract_pos_of_relay_on_directed_paths(
    const std::map<std::tuple<int, int, int>, std::array<int, 4>>
        &nodes_on_directed_paths) const
{
    EV << "SimpleMISToCDSAlg::extract_pos_of_relay_on_directed_paths()\n";
    std::map<std::tuple<int, int, int>, int> pos_of_relay_on_directed_paths_1;
    for (const auto &it : nodes_on_directed_paths) {
        std::tuple<int, int, int> edge = it.first;
        std::array<int, 4> path = it.second;
        EV << tuple_to_string(edge) << '\n';
        int pos = extract_pos_of_relay_on_directed_path(path);
        EV << '\t' << pos << '\n';
        if (pos > 0) pos_of_relay_on_directed_paths_1[edge] = pos;
    }
    return pos_of_relay_on_directed_paths_1;
}

int SimpleMISToCDSAlg::extract_pos_of_relay_on_directed_path(
    const std::array<int, 4> &path) const
{
    if (std::get<1>(path) == id) return 1;
    if (std::get<2>(path) == id) return 2;
    return -1;
}

void SimpleMISToCDSAlg::print_state(int log_level)
{
    EV << "SimpleMISToCDSAlg::print_state()\n";
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "state = {\n";
    s += (tab + one_tab + "nodes_on_path = {" + "\n");
    for (auto it : nodes_on_path) {
        s += (tab + one_tab + one_tab + std::to_string(it.first) + ": " +
              std::to_string(it.second[1]) + ",\n");
    }
    s += (tab + one_tab + "}\n");

    s += (tab + one_tab + "nodes_on_directed_paths = {" + "\n");
    for (auto it : nodes_on_directed_paths) {
        s += (tab + one_tab + one_tab + tuple_to_string(it.first) + ": " +
              array_to_string<int>(it.second) + ",\n");
    }
    s += (tab + one_tab + "}\n");

    s += tab + "}";
    EV << s << '\n';
}