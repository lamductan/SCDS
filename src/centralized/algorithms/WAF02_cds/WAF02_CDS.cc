#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>

#include "centralized/algorithms/WAF02_cds/WAF02_CDS.h"
#include "centralized/algorithms/checkers/cds_checker.h"
#include "centralized/algorithms/checkers/two_hop_mis_checker.h"
#include "utils/utils.h"

centralized::WAF02CDSAlg::WAF02CDSAlg() {}

centralized::WAF02CDSAlg::WAF02CDSAlg(std::map<int, centralized::Node *> nodes,
                                      std::set<centralized::Edge> edges)
{
    this->graph = new Graph(nodes, edges, CDS_WAF02_NODE_TYPE);
    for (auto it : graph->nodes) {
        int nodeid = it.first;
        WAF02CDSNode *WAF02_cds_node = dynamic_cast<WAF02CDSNode *>(it.second);
        WAF02_cds_nodes[nodeid] = WAF02_cds_node;
    }
}

std::map<int, centralized::BFSTreeStructure>
centralized::WAF02CDSAlg::find_bfs_tree_structure_from_root(int root)
{
    std::cout
        << "centralized::WAF02CDSAlg::find_bfs_tree_structure_from_root()\n";
    std::map<int, BFSTreeStructure> bfs_tree_structure_all_nodes;
    std::map<int, bool> vis;
    std::map<int, int> level;
    std::queue<std::pair<int, int>> q;
    q.push({ root, -1 });
    while (!q.empty()) {
        std::pair<int, int> front = q.front();
        q.pop();
        int u = front.first;
        int parent = front.second;
        if (vis[u])
            continue;
        vis[u] = true;
        if (u == root)
            level[u] = 0;
        else
            level[u] = level[parent] + 1;
        bfs_tree_structure_all_nodes[u] = BFSTreeStructure(u, level[u], parent);
        if (parent != -1)
            bfs_tree_structure_all_nodes[parent].children.insert(u);
        Node *node_u = graph->nodes[u];
        for (auto it : node_u->neighbors) {
            int v = it.first;
            if (v == parent)
                continue;
            if (vis[v])
                continue;
            q.push({ v, u });
        }
    }
    bfs_tree_structures = bfs_tree_structure_all_nodes;

    for (auto it : WAF02_cds_nodes) {
        int nodeid = it.first;
        WAF02CDSNode *WAF02_cds_node = it.second;
        WAF02_cds_node->bfs_tree_structure =
            bfs_tree_structure_all_nodes[nodeid];
    }

    for (auto it : WAF02_cds_nodes) {
        int nodeid = it.first;
        // std::cout << "\t" << "node" << nodeid << '\n';
        WAF02CDSNode *WAF02_cds_node = it.second;
        int l = WAF02_cds_node->bfs_tree_structure.level;
        WAF02_cds_node->y = WAF02_cds_node->neighbors.size();
        // std::cout << "\t\t" << "l = " << l << ", y = " << WAF02_cds_node->y
        // << "\n";
        for (auto it1 : it.second->neighbors) {
            int neighbor_id = it1.first;
            WAF02CDSNode *WAF02_cds_neighbor_node =
                WAF02_cds_nodes[neighbor_id];
            int l1 = WAF02_cds_neighbor_node->bfs_tree_structure.level;
            if (l1 > l || (l1 == l && neighbor_id > nodeid))
                --WAF02_cds_node->y;
            // std::cout << "\t\t\t" << "neighbor_id = " << neighbor_id << ", l1
            // = " << l1 << ", y = "
            //           << WAF02_cds_node->y << '\n';
        }
    }
    WAF02_cds_nodes[root]->x2 = WAF02_cds_nodes[root]->neighbors.size();

    return bfs_tree_structure_all_nodes;
}

std::vector<int> centralized::WAF02CDSAlg::find_mis(bool print_undediced_node)
{
    std::cout << "centralized::WAF02CDSAlg::find_mis()\n";
    find_bfs_tree_structure_from_root(root);
    graph->find_two_hop_neighbors();

    std::vector<int> mis_nodes;
    std::set<std::pair<int, int>> order;
    for (auto it : WAF02_cds_nodes) {
        int nodeid = it.first;
        WAF02CDSNode *WAF02_cds_node = it.second;
        int level = WAF02_cds_node->bfs_tree_structure.level;
        order.insert({ level, nodeid });
    }

    while (!order.empty()) {
        std::pair<int, int> lowest_rank = *order.begin();
        order.erase(order.begin());
        int nodeid = lowest_rank.second;
        WAF02CDSNode *lowest_rank_node = WAF02_cds_nodes[nodeid];
        lowest_rank_node->MIS_status = IN_MIS;
        mis_nodes.push_back(nodeid);
        // std::cout << "\t" << "select node" << nodeid << ", level = " <<
        // lowest_rank.first << '\n';
        for (auto it : lowest_rank_node->neighbors) {
            int neighbor_id = it.first;
            WAF02CDSNode *neighbor_node = WAF02_cds_nodes[neighbor_id];
            neighbor_node->MIS_status = NOT_IN_MIS;
            int neighbor_level = neighbor_node->bfs_tree_structure.level;
            order.erase({ neighbor_level, neighbor_id });
        }
    }

    TwoHopMISChecker *mis_checker = new TwoHopMISChecker(this, mis_nodes);
    assert(mis_checker->check());
    delete mis_checker;
    return mis_nodes;
}

std::set<int> centralized::WAF02CDSAlg::find_cds_nodes_from_mis_nodes(
    const std::vector<int> &mis_nodes)
{
    std::cout << "centralized::WAF02CDSAlg::find_cds_nodes_from_mis_nodes()\n";
    int root_T_star = find_root_T_star(root);
    std::set<int> cds_nodes_set(mis_nodes.begin(), mis_nodes.end());
    cds_nodes_set.insert(root_T_star);
    WAF02CDSNode *root_T_star_node = WAF02_cds_nodes[root_T_star];
    for (int mis_node_id : mis_nodes) {
        if (root_T_star_node->neighbors.count(mis_node_id))
            continue;
        WAF02CDSNode *WAF02_mis_node = WAF02_cds_nodes[mis_node_id];
        cds_nodes_set.insert(WAF02_mis_node->bfs_tree_structure.parent);
    }
    return cds_nodes_set;
}

int centralized::WAF02CDSAlg::find_root_T_star(int root)
{
    std::cout << "centralized::WAF02CDSAlg::find_root_T_star()\n";
    int max_n_mis_neighbors = 0;
    int root_T_star = -1;
    WAF02CDSNode *root_node = WAF02_cds_nodes[root];
    for (auto it : root_node->neighbors) {
        int neighbor_id = it.first;
        WAF02CDSNode *neighbor_node = WAF02_cds_nodes[neighbor_id];
        int n_mis_neighbors = count_mis_neighbor(neighbor_node);
        // std::cout << "\t" << "neighbor_id = " << neighbor_id << ", " <<
        // "n_mis_neighbors = " << n_mis_neighbors << '\n';
        if (n_mis_neighbors > max_n_mis_neighbors) {
            max_n_mis_neighbors = n_mis_neighbors;
            root_T_star = neighbor_id;
        }
    }
    return root_T_star;
}

int centralized::WAF02CDSAlg::count_mis_neighbor(WAF02CDSNode *node)
{
    int n_mis_neighbors = 0;
    for (auto it : node->neighbors) {
        Node *neighbor_node = it.second;
        if (neighbor_node->MIS_status == IN_MIS)
            ++n_mis_neighbors;
    }
    return n_mis_neighbors;
}