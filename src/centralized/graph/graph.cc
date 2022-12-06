#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "centralized/graph/graph.h"
#include "centralized/graph/node_factory.h"
#include "utils/union_find.h"
#include "utils/utils.h"

centralized::Graph::Graph(const char *network_file)
{
    std::string line;
    std::fstream networkFile(network_file, std::ios::in);
    while (getline(networkFile, line, '\n')) {
        if (line.empty() || line[0] == '#') continue;

        std::vector<std::string> tokens = splitString(line);

        n = atoi(tokens[0].c_str());
        m = atoi(tokens[1].c_str());
        break;
    }

    for (int i = 0; i < n; ++i) {
        getline(networkFile, line, '\n');
        std::vector<std::string> tokens = splitString(line);
        int nodeid = atoi(tokens[0].c_str());
        int x = atoi(tokens[1].c_str());
        int y = atoi(tokens[2].c_str());
        Node *node = new Node(x, y, nodeid);
        nodes[nodeid] = node;
        max_node_id = std::max(max_node_id, nodeid);
    }

    // read and create connections
    for (int i = 0; i < m; ++i) {
        getline(networkFile, line, '\n');
        std::vector<std::string> tokens = splitString(line);
        int srcnodeid = atoi(tokens[0].c_str());
        int destnodeid = atoi(tokens[1].c_str());

        add_edge(Edge(srcnodeid, destnodeid));

        Node *srcnode = nodes[srcnodeid];
        Node *destnode = nodes[destnodeid];

        srcnode->add_neighbor(destnode);
        destnode->add_neighbor(srcnode);
    }

    std::cout << "Done buildNetwork " << '\n';
    networkFile.close();
}

centralized::Graph::Graph(const std::map<int, Node *> &nodes,
                          const std::set<Edge> &edges,
                          centralized::NodeType node_type)
{
    n = nodes.size();
    m = edges.size();
    this->nodes = std::map<int, Node *>();
    for (auto it : nodes) {
        int nodeid = it.first;
        this->nodes[nodeid] = NodeFactory::create_node(it.second, node_type);
        max_node_id = std::max(max_node_id, nodeid);
    }
    for (Edge edge : edges) {
        add_edge(edge);
    }
}

centralized::Graph::Graph(const std::set<int> &nodes,
                          const std::set<std::tuple<int, int, int>> &edges)
{
    n = nodes.size();
    m = edges.size();
    this->nodes = std::map<int, Node *>();
    for (int nodeid : nodes) {
        this->nodes[nodeid] = new Node(0, 0, nodeid);
        max_node_id = std::max(max_node_id, nodeid);
    }
    for (const std::tuple<int, int, int> &edge : edges) {
        add_edge(edge);
    }
}

centralized::Graph::Graph(const centralized::Graph &other)
{
    copyInformation(other.n, other.m, other.nodes);
}

centralized::Graph &
centralized::Graph::operator=(const centralized::Graph &other)
{
    copyInformation(other.n, other.m, other.nodes);
    return *this;
}

void centralized::Graph::add_edge(Edge edge)
{
    if (!nodes.count(edge.u)) return;
    if (!nodes.count(edge.v)) return;
    edges.insert(edge);
    Node *srcnode = nodes[edge.u];
    Node *destnode = nodes[edge.v];
    srcnode->add_neighbor(destnode);
    destnode->add_neighbor(srcnode);
}

void centralized::Graph::add_edge(const std::tuple<int, int, int> &edge_id)
{
    int u = std::get<1>(edge_id);
    int v = std::get<2>(edge_id);
    int w = std::get<0>(edge_id);
    if (!nodes.count(u)) return;
    if (!nodes.count(v)) return;
    Edge edge = Edge(u, v, w);
    return add_edge(edge);
}

void centralized::Graph::copyInformation(int n, int m,
                                         std::map<int, Node *> nodes)
{
    this->n = n;
    this->m = m;
    this->nodes = std::map<int, Node *>();
    for (auto it : nodes) {
        int nodeid = it.first;
        this->max_node_id = std::max(this->max_node_id, nodeid);
        this->nodes[nodeid] = it.second->dup();
    }
}

bool centralized::Graph::is_connected()
{
    std::map<int, bool> visited;
    dfs(nodes.begin()->first, -1, visited);
    for (auto it : nodes)
        if (!visited[it.first]) return false;
    return true;
}

void centralized::Graph::dfs(int u, int parent, std::map<int, bool> &visited)
{
    if (visited[u]) return;
    // std::cout << "\tvisit " << u << '\n';
    visited[u] = true;
    for (auto it : nodes[u]->neighbors) {
        int neighbor_id = it.first;
        if (neighbor_id == parent) continue;
        if (visited[neighbor_id]) continue;
        dfs(neighbor_id, u, visited);
    }
}

void centralized::Graph::print() const
{
    std::cout << "Graph::n = " << n << '\n';
    for (auto it : nodes) {
        centralized::Node *node = it.second;
        std::cout << node->to_string() << '\n';
        for (auto it : node->neighbors) {
            centralized::Node *neighbor = it.second;
            std::cout << "\t" << neighbor->to_string() << '\n';
        }
    }
}

int centralized::Graph::compute_total_weight() const
{
    int total_weight = 0;
    for (Edge edge : edges) total_weight += edge.w;
    return total_weight;
}

std::vector<centralized::Edge> centralized::Graph::build_mst() const
{
    UnionFind union_find(max_node_id + 1);
    std::vector<Edge> edges_vector(edges.begin(), edges.end());
    std::sort(edges_vector.begin(), edges_vector.end(),
              [](Edge a, Edge b) { return (a.w < b.w); });

    std::vector<Edge> mst_edges;
    for (Edge edge : edges_vector) {
        if (mst_edges.size() < n - 1) {
            int fr = edge.u;
            int to = edge.v;
            if (union_find.find(fr) == union_find.find(to)) continue;
            union_find.join(fr, to);
            mst_edges.push_back(edge);
        } else {
            break;
        }
    }
    return mst_edges;
}

void centralized::Graph::find_two_hop_neighbors_of_a_node(Node *node)
{
    for (auto it : node->neighbors) {
        Node *one_hop_neighbor = it.second;
        for (auto it1 : one_hop_neighbor->neighbors) {
            Node *two_hop_neighbor = it1.second;
            if (two_hop_neighbor->id == node->id) continue;
            if (node->neighbors.count(two_hop_neighbor->id) > 0) continue;
            if (node->two_hop_neighbors.count(two_hop_neighbor) > 0) continue;
            node->two_hop_neighbors[two_hop_neighbor] = { one_hop_neighbor,
                                                          nullptr };
        }
    }
}

void centralized::Graph::find_two_hop_neighbors()
{
    for (auto it : nodes) {
        Node *node = it.second;
        find_two_hop_neighbors_of_a_node(node);
    }
}

void centralized::Graph::find_three_hop_neighbors_of_a_node(Node *node)
{
    find_two_hop_neighbors_of_a_node(node);
    for (auto it : node->two_hop_neighbors) {
        Node *two_hop_neighbor = it.first;
        std::array<Node *, 2> one_hop_path = it.second;
        for (auto it1 : two_hop_neighbor->neighbors) {
            Node *three_hop_neighbor = it1.second;
            if (three_hop_neighbor->id == node->id) continue;
            if (node->neighbors.count(three_hop_neighbor->id) > 0) continue;
            if (node->two_hop_neighbors.count(three_hop_neighbor) > 0) continue;
            if (node->three_hop_neighbors.count(three_hop_neighbor) > 0)
                continue;
            node->three_hop_neighbors[three_hop_neighbor] = {
                one_hop_path[0], two_hop_neighbor
            };
        }
    }
}

void centralized::Graph::find_three_hop_neighbors()
{
    std::cout << "centralized::Graph::find_three_hop_neighbors()\n";
    for (auto it : nodes) {
        Node *node = it.second;
        // std::cout << "\tnode" << node->id << '\n';
        find_three_hop_neighbors_of_a_node(node);
    }
}

std::map<int, centralized::BFSTreeStructure>
centralized::Graph::find_bfs_tree_structure_from_root(int root)
{
    std::map<int, BFSTreeStructure> bfs_tree_structure_all_nodes;
    std::map<int, bool> vis;
    std::map<int, int> level;
    std::set<std::tuple<int, int, int>> q;
    q.insert({ 0, root, -1 });
    while (!q.empty()) {
        std::tuple<int, int, int> front = *q.begin();
        q.erase(q.begin());
        int l = std::get<0>(front);
        int u = std::get<1>(front);
        int parent = std::get<2>(front);
        if (vis[u]) continue;
        vis[u] = true;
        level[u] = l;
        bfs_tree_structure_all_nodes[u] = BFSTreeStructure(u, l, parent, root);
        if (parent != -1)
            bfs_tree_structure_all_nodes[parent].children.insert(u);
        Node *node_u = nodes[u];
        for (auto it : node_u->neighbors) {
            int v = it.first;
            if (v == parent) continue;
            if (vis[v]) continue;
            q.insert({ l + 1, v, u });
        }
    }
    return bfs_tree_structure_all_nodes;
}

int centralized::Graph::get_max_level(
    const std::map<int, centralized::BFSTreeStructure> &bfs_tree_structures)
{
    int max_level = 0;
    for (auto it : bfs_tree_structures) {
        BFSTreeStructure bfs_tree_structure = it.second;
        max_level = std::max(max_level, bfs_tree_structure.level);
    }
    return max_level;
}

centralized::Graph::~Graph()
{
    for (auto it : nodes) delete it.second;
}