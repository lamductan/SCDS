#include "checkers/mst_checker.h"

#include <set>
#include <unordered_map>

#include "algorithms/ialg_node.h"

using namespace omnetpp;

MSTChecker::MSTChecker(Network *network) : IChecker(network)
{
    selected_nodes = network->get_selected_nodes();
    selected_nodes_set =
        std::set<int>(selected_nodes.begin(), selected_nodes.end());
    for (int selected_node_id : selected_nodes) {
        Node *node = network->nodes[selected_node_id];
        centralized::Node *centralized_node =
            new centralized::Node(0, 0, node->id);
        selected_nodes_map[selected_node_id] = centralized_node;
    }

    for (auto it : network->nodes) {
        Node *node = it.second;
        centralized::Node *centralized_node =
            new centralized::Node(0, 0, node->id);
        all_nodes_map[node->id] = centralized_node;
    }

    subgraph_from_original_graph_edges =
        construct_subgraph_from_original_graph();
    std::vector<centralized::Edge> mst_edges_original_graph =
        subgraph_from_original_graph_edges->build_mst();
    minimal_weight_tree_of_subgraph_from_original_graph = 0;
    for (const centralized::Edge &edge : mst_edges_original_graph)
        minimal_weight_tree_of_subgraph_from_original_graph += edge.w;

    subgraph_from_alg_edges = construct_subgraph_from_alg();
    minimal_weight_tree_of_subgraph_from_alg =
        subgraph_from_alg_edges->compute_total_weight();
}

centralized::Graph *MSTChecker::construct_subgraph_from_original_graph()
{
    std::set<centralized::Edge> edges_from_original_graph;
    for (auto it : selected_nodes_map) {
        int nodeid = it.first;
        centralized::Node *centralized_node = it.second;
        Node *node = network->nodes[nodeid];
        for (int neighbor_id : node->all_neighbors) {
            if (neighbor_id < nodeid)
                continue;
            if (selected_nodes_set.count(neighbor_id)) {
                edges_from_original_graph.insert({ nodeid, neighbor_id });
            }
        }
    }

    // A dirty trick: We are going to find MST of an MIS, so edges are not
    // actually in the original graph
    if (edges_from_original_graph.empty()) {
        original_graph = construct_original_graph();
        original_graph->find_three_hop_neighbors();
        for (int u : selected_nodes) {
            centralized::Node *node_u = original_graph->nodes[u];
            for (int v : selected_nodes) {
                if (v <= u)
                    continue;
                centralized::Node *node_v = original_graph->nodes[v];
                if (node_u->two_hop_neighbors.count(node_v) > 0) {
                    edges_from_original_graph.insert({ u, v, 1 });
                } else if (node_u->three_hop_neighbors.count(node_v) > 0) {
                    edges_from_original_graph.insert({ u, v, 2 });
                }
            }
        }
    }

    return new centralized::Graph(selected_nodes_map, edges_from_original_graph,
                                  centralized::CDS_SIMPLE_NODE_TYPE);
}

centralized::Graph *MSTChecker::construct_subgraph_from_alg()
{
    std::set<centralized::Edge> edges_from_alg;
    for (int u : selected_nodes) {
        Node *node = network->nodes[u];
        for (auto it : node->alg->tree_edges) {
            int v = it.first;
            if (v < u)
                continue;
            edges_from_alg.insert(it.second);
        }
    }
    return new centralized::Graph(selected_nodes_map, edges_from_alg,
                                  centralized::CDS_SIMPLE_NODE_TYPE);
}

centralized::Graph *MSTChecker::construct_original_graph()
{
    std::set<centralized::Edge> edges_from_original_graph;
    for (auto it : all_nodes_map) {
        int nodeid = it.first;
        centralized::Node *centralized_node = it.second;
        Node *node = network->nodes[nodeid];
        for (int neighbor_id : node->all_neighbors) {
            if (neighbor_id < nodeid)
                continue;
            edges_from_original_graph.insert({ nodeid, neighbor_id });
        }
    }
    return new centralized::Graph(all_nodes_map, edges_from_original_graph,
                                  centralized::CDS_SIMPLE_NODE_TYPE);
}

bool MSTChecker::check_connected(bool is_final_check) const
{
    std::cout << "MSTChecker::check_connected()\n";
    bool res = subgraph_from_alg_edges->is_connected();
    if (res)
        std::cout << "\tPASS\n";
    else
        std::cout << "\tFAILED\n";
    return res;
}

bool MSTChecker::check(bool is_final_check) const
{
    std::cout << "MSTChecker::check()\n";
    if (!check_all_decided(is_final_check)) {
        if (is_final_check)
            EV_ERROR << "Failed ALL DECIDED check!\n";
        return false;
    }
    if (subgraph_from_alg_edges->edges.size() !=
        subgraph_from_alg_edges->nodes.size() - 1) {
        if (is_final_check)
            EV_ERROR << "Failed NUMBER OF SPANNING TREE EDGES check!\n";
        return false;
    }
    if (minimal_weight_tree_of_subgraph_from_alg !=
        minimal_weight_tree_of_subgraph_from_original_graph) {
        if (is_final_check)
            EV_ERROR << "Failed WEIGHT check!\n";
        return false;
    }
    if (!check_connected(is_final_check)) {
        if (is_final_check)
            EV_ERROR << "Failed CONNECTED check!\n";
        return false;
    }
    EV << "PASS MST CHECK!\n";
    return true;
}

MSTChecker::~MSTChecker()
{
    for (auto it : selected_nodes_map)
        delete it.second;
    for (auto it : all_nodes_map)
        delete it.second;
    delete subgraph_from_original_graph_edges;
    delete subgraph_from_alg_edges;
    delete original_graph;
}