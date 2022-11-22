#include <algorithm>
#include <iostream>
#include <cassert>

#include "utils/utils.h"
#include "centralized/algorithms/cds_simple/CDS_simple.h"
#include "centralized/algorithms/cds_simple/CDS_simple_node.h"
#include "centralized/algorithms/checkers/mis_checker.h"
#include "centralized/algorithms/checkers/cds_checker.h"

centralized::CDSSimpleAlg::CDSSimpleAlg() {}

centralized::CDSSimpleAlg::CDSSimpleAlg(std::map<int, centralized::Node *> nodes, std::set<centralized::Edge> edges) {
    this->graph = new Graph(nodes, edges, CDS_SIMPLE_NODE_TYPE);
}

bool centralized::CDSSimpleAlg::check_all_nodes_decided_mis_status(bool print_undediced_node) {
    std::set<Node *> undecided_nodes;
    for(auto it : graph->nodes) {
        Node *node = it.second;
        if (!node->is_mis_status_decided()) {
            undecided_nodes.insert(node);
        }
    }

    bool res = undecided_nodes.empty();
    if (print_undediced_node) {
        if (res) {
            std::cout << "All nodes decided MIS status\n";
        } else {
            //std::cout << "Undediced MIS status nodes: [";
            //for(Node *node : undecided_nodes) std::cout << node->id << ',';
            //std::cout << "]\n";
        }
    }
    return res;
}

std::vector<int> centralized::CDSSimpleAlg::find_mis(bool print_undediced_node) {
    std::vector<int> mis_nodes;
    for(auto it : graph->nodes) {
        Node *node = it.second;
        node->all_remained_neighbors = node->neighbors;
    }

    int cnt = 0;
    while (!check_all_nodes_decided_mis_status(print_undediced_node)) {
        ++cnt;
        if (cnt > graph->n) {
            assert(false);
        }
        for(auto it : graph->nodes) {
            Node *node = it.second;
            if (node->is_mis_status_decided()) continue;
            if (node->all_remained_neighbors.empty()) {
                node->MIS_status = IN_MIS;
                node->dominator = node->id;
            } else if (node->id < node->all_remained_neighbors.begin()->first) {
                node->MIS_status = IN_MIS;
                node->dominator = node->id;
            } else {
                std::vector<Node *> not_in_mis_neighbors;
                for(auto it1 : node->all_remained_neighbors) {
                    Node *neighbor = it1.second;
                    if (neighbor->MIS_status == IN_MIS) {
                        node->MIS_status = NOT_IN_MIS;
                        node->dominator = neighbor->id;
                        break;
                    } else if (neighbor->MIS_status == NOT_IN_MIS) {
                        not_in_mis_neighbors.push_back(neighbor);
                    }
                }
                for(Node *neighbor : not_in_mis_neighbors) 
                    node->all_remained_neighbors.erase(neighbor->id);
            }
        }
    }

    for(auto it : graph->nodes) {
        Node *node = it.second;
        if (node->MIS_status == IN_MIS) mis_nodes.push_back(node->id);
    }
    std::sort(mis_nodes.begin(), mis_nodes.end());

    MISChecker *mis_checker = new MISChecker(this, mis_nodes);
    assert(mis_checker->check());
    delete mis_checker;
    return mis_nodes;
}

std::vector<int> centralized::CDSSimpleAlg::find_cds() {
    std::vector<int> mis_nodes = find_mis();
    std::set<int> cds_nodes_set = find_cds_nodes_from_mis_nodes(mis_nodes);
    
    cds_nodes_set = apply_rule_1(cds_nodes_set);
    cds_nodes_set = apply_rule_2(cds_nodes_set);

    std::vector<int> cds_nodes(cds_nodes_set.begin(), cds_nodes_set.end());
    CDSChecker *cds_checker = new CDSChecker(this, cds_nodes);
    bool res = cds_checker->check();
    std::cout << "cds_checker->check() = " << res << "\n";
    assert(res);
    return cds_nodes;
}

void centralized::CDSSimpleAlg::find_two_hop_neighbors_of_a_node(Node *node) {
    for(auto it : node->neighbors) {
        Node *one_hop_neighbor = it.second;
        for(auto it1 : one_hop_neighbor->neighbors) {
            Node *two_hop_neighbor = it1.second;
            if (two_hop_neighbor->id == node->id) continue;
            if (node->neighbors.count(two_hop_neighbor->id) > 0) continue;
            if (node->two_hop_neighbors.count(two_hop_neighbor) > 0) continue;
            node->two_hop_neighbors[two_hop_neighbor] = {one_hop_neighbor, nullptr};
        }
    }
}

void centralized::CDSSimpleAlg::find_two_hop_neighbors() {
    for(auto it : graph->nodes) {
        Node *node = it.second;
        find_two_hop_neighbors_of_a_node(node);
    }
}

void centralized::CDSSimpleAlg::find_three_hop_neighbors_of_a_node(Node *node) {
    find_two_hop_neighbors_of_a_node(node);
    CDSSimpleNode *cds_simple_node = dynamic_cast<CDSSimpleNode *>(node);
    for(auto it : cds_simple_node->two_hop_neighbors) {
        Node *two_hop_neighbor = it.first;
        std::array<Node *, 2> one_hop_path = it.second;
        for(auto it1 : two_hop_neighbor->neighbors) {
            Node *three_hop_neighbor = it1.second;
            if (three_hop_neighbor->id == node->id) continue;
            if (node->neighbors.count(three_hop_neighbor->id) > 0) continue;
            if (cds_simple_node->two_hop_neighbors.count(three_hop_neighbor) > 0) continue;
            if (cds_simple_node->three_hop_neighbors.count(three_hop_neighbor) > 0) continue;
            cds_simple_node->three_hop_neighbors[three_hop_neighbor] = {one_hop_path[0], two_hop_neighbor};
        }
    }
}

void centralized::CDSSimpleAlg::find_three_hop_neighbors() {
    std::cout << "centralized::CDSSimpleAlg::find_three_hop_neighbors()\n";
    for(auto it : graph->nodes) {
        Node *node = it.second;
        //std::cout << "\tnode" << node->id << '\n';
        find_three_hop_neighbors_of_a_node(node);
        CDSSimpleNode *cds_simple_node = dynamic_cast<CDSSimpleNode *>(node);
        cds_simple_node->two_three_hop_neighbors = combine_map<Node *, std::array<Node *, 2>>(
            cds_simple_node->two_hop_neighbors, cds_simple_node->three_hop_neighbors
        );
    }
}

std::set<int> centralized::CDSSimpleAlg::find_cds_nodes_from_mis_nodes(const std::vector<int> &mis_nodes) {
    std::cout << "centralized::CDSSimpleAlg::find_cds_nodes_from_nis_nodes()\n";
    find_three_hop_neighbors();
    std::set<int> mis_nodes_set(mis_nodes.begin(), mis_nodes.end());
    std::set<int> cds_nodes_set(mis_nodes.begin(), mis_nodes.end());
    for(int v : mis_nodes) {
        //std::cout << "\tnode " << v << '\n';
        Node *node = graph->nodes[v];
        CDSSimpleNode *cds_simple_node = dynamic_cast<CDSSimpleNode *>(node);
        for(auto it : cds_simple_node->two_three_hop_neighbors) {
            Node *two_three_hop_neighbor = it.first;
            int two_three_hop_neighbor_id = two_three_hop_neighbor->id;
            if (two_three_hop_neighbor_id > node->id) continue;
            if (mis_nodes_set.count(two_three_hop_neighbor_id) == 0) continue;
            CDSSimpleNode *cds_simple_two_three_hop_neighbor = dynamic_cast<CDSSimpleNode *>(two_three_hop_neighbor);
            std::array<Node *, 2> path = it.second;
            for(int i = 0; i < 2; ++i) {
                if (path[i] == nullptr) continue;
                //std::cout << "\tAdding " << path[i]->id << " as a node on path from "
                //   << two_three_hop_neighbor_id << " to " << v << '\n';
                cds_nodes_set.insert(path[i]->id);
            }
        }
    }
    return cds_nodes_set;
}

bool centralized::CDSSimpleAlg::rule1_condition1(int u, int v) {
    std::set<int> N_u = graph->nodes[u]->neighbors_set;
    std::set<int> N_v = graph->nodes[v]->neighbors_set;
    return is_subset(N_u, N_v);
}

bool centralized::CDSSimpleAlg::rule1_condition2(int u, int v) {
    if (u > v) return false;
    std::set<int> N_u = graph->nodes[u]->neighbors_set;
    std::set<int> N_v = graph->nodes[v]->neighbors_set;
    return N_u == N_v;
}

bool centralized::CDSSimpleAlg::rule2_condition1(int u, int v, int w) {
    std::set<int> N_u = graph->nodes[u]->neighbors_set;
    
    std::set<int> N_v = graph->nodes[v]->neighbors_set;
    std::set<int> N_w = graph->nodes[w]->neighbors_set;
    std::set<int> N_vw = combine_set<int>(N_v, N_w);
    N_vw.erase(u);
    
    return is_subset(N_u, N_vw);
}

bool centralized::CDSSimpleAlg::rule2_condition2(int u, int v, int w) {
    if (u > v || u > w) return false;
    std::set<int> N_u = graph->nodes[u]->neighbors_set;
    N_u.erase(v); N_u.erase(w);

    std::set<int> N_v = graph->nodes[v]->neighbors_set;
    std::set<int> N_w = graph->nodes[w]->neighbors_set;
    std::set<int> N_vw = combine_set<int>(N_v, N_w);
    N_vw.erase(u); N_vw.erase(v); N_vw.erase(w);
    
    return (N_u == N_vw);
}

bool centralized::CDSSimpleAlg::rule3_condition1(int u, int x, int y, int z) {
    std::set<int> N_u = graph->nodes[u]->neighbors_set;
    
    std::set<int> N_x = graph->nodes[x]->neighbors_set;
    std::set<int> N_y = graph->nodes[y]->neighbors_set;
    std::set<int> N_z = graph->nodes[z]->neighbors_set;
    std::set<int> N_xyz = combine_set<int>(combine_set<int>(N_x, N_y), N_z);
    N_xyz.erase(u);
    
    return is_subset(N_u, N_xyz);
}

bool centralized::CDSSimpleAlg::rule3_condition2(int u, int x, int y, int z) {
    if (u > x || u > y || u > z) return false;
    std::set<int> N_u = graph->nodes[u]->neighbors_set;
    N_u.erase(x); N_u.erase(y); N_u.erase(z);

    std::set<int> N_x = graph->nodes[x]->neighbors_set;
    std::set<int> N_y = graph->nodes[y]->neighbors_set;
    std::set<int> N_z = graph->nodes[z]->neighbors_set;
    std::set<int> N_xyz = combine_set<int>(combine_set<int>(N_x, N_y), N_z);
    N_xyz.erase(u); N_xyz.erase(x); N_xyz.erase(y); N_xyz.erase(z);
    
    return (N_u == N_xyz);
}

std::set<int> centralized::CDSSimpleAlg::apply_rule_1(const std::set<int> &cds_nodes) {
    //std::cout << "centralized::CDSSimpleAlg::apply_rule_1()\n";
    std::set<int> cds_nodes_1 = cds_nodes;
    for(int u : cds_nodes_1) {
        //std::cout << "Examining node" << u << '\n';
        bool erase_u = false;
        Node *node_u = graph->nodes[u];
        
        //std::cout << "\t";
        //char name_u[10];
        //snprintf(name_u, 10, "N[%4d]", u);
        //print<std::set<int>>(node_u->neighbors_set, name_u);
        //std::cout << "\n";
        
        for(int v : node_u->neighbors_set) {
            if (cds_nodes_1.count(v) == 0) continue;
            Node *node_v = graph->nodes[v];
            
            //std::cout << "\t";
            //char name_v[10];
            //snprintf(name_v, 10, "N[%4d]", v);
            //print<std::set<int>>(node_v->neighbors_set, name_v);
            //std::cout << "\n";
            
            bool cond1 = rule1_condition1(u, v);
            bool cond2 = rule1_condition2(u, v);

            //if (cond1) std::cout << "\t" << "Rule1 condition 1 satisfied\n";
            //else if (cond2) std::cout << "\t" << "Rule1 condition 2 satisfied\n";

            if (cond1 || cond2) {
                cds_nodes_1.erase(u);
                erase_u = true;
                break;
            }
        }
        
        //if (erase_u) {
        //    std::cout << "\t" << "Erase node" << u << '\n';
        //} else {
        //    std::cout << "\t" << "Keep node" << u << '\n';
        //}
    }
    return cds_nodes_1;
}

std::set<int> centralized::CDSSimpleAlg::apply_rule_2(const std::set<int> &cds_nodes) {
    //std::cout << "centralized::CDSSimpleAlg::apply_rule_2()\n";
    std::set<int> cds_nodes_1 = cds_nodes;
    for(int u : cds_nodes_1) {
        //std::cout << "Examining node" << u << '\n';
        bool erase_u = false;
        Node *node_u = graph->nodes[u];
        
        //std::cout << "\t";
        //char name_u[10];
        //snprintf(name_u, 10, "N[%4d]", u);
        //print<std::set<int>>(node_u->neighbors_set, name_u);
        //std::cout << "\n";
        
        for(int v : node_u->neighbors_set) {
            if (cds_nodes_1.count(v) == 0) continue;
            Node *node_v = graph->nodes[v];
            for(int w : node_u->neighbors_set) {
                if (cds_nodes_1.count(w) == 0) continue;
                Node *node_w = graph->nodes[w];
                
                //std::cout << "\t";
                //char name_v[10];
                //snprintf(name_v, 10, "N[%4d]", v);
                //print<std::set<int>>(node_v->neighbors_set, name_v);
                //std::cout << "\n";
                
                //std::cout << "\t";
                //char name_w[10];
                //snprintf(name_w, 10, "N[%4d]", w);
                //print<std::set<int>>(node_w->neighbors_set, name_w);
                //std::cout << "\n";
                
                bool cond1 = rule2_condition1(u, v, w);
                bool cond2 = rule2_condition2(u, v, w);

                //if (cond1) std::cout << "\t" << "Rule2 condition 1 satisfied\n";
                //else if (cond2) std::cout << "\t" << "Rule2 condition 2 satisfied\n";

                if (cond1 || cond2) {
                    cds_nodes_1.erase(u);
                    erase_u = true;
                    break;
                }
            }
            if (erase_u) break;
        }

        //if (erase_u) {
        //    std::cout << "\t" << "Erase node" << u << '\n';
        //} else {
        //    std::cout << "\t" << "Keep node" << u << '\n';
        //}
    }
    return cds_nodes_1;
}

centralized::CDSSimpleAlg::~CDSSimpleAlg() {
    delete graph;
}