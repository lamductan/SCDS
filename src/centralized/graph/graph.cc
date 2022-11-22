#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "utils/utils.h"
#include "centralized/graph/graph.h"
#include "centralized/graph/node_factory.h"

centralized::Graph::Graph(const char *network_file) {
    std::string line;
    std::fstream networkFile(network_file, std::ios::in);
    while (getline(networkFile, line, '\n')) {
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> tokens = splitString(line);

        n = atoi(tokens[0].c_str());
        m = atoi(tokens[1].c_str());
        break;
    }

    for(int i = 0; i < n; ++i) {
        getline(networkFile, line, '\n');
        std::vector<std::string> tokens = splitString(line);
        int nodeid = atoi(tokens[0].c_str());
        int x = atoi(tokens[1].c_str());
        int y = atoi(tokens[2].c_str());
        Node *node = new Node(x, y, nodeid);
        nodes[nodeid] = node;
    }

    // read and create connections
    for(int i = 0; i < m; ++i) {
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

centralized::Graph::Graph(std::map<int, Node *> nodes, std::set<Edge> edges, centralized::NodeType node_type) {
    n = nodes.size();
    m = edges.size();
    this->nodes = std::map<int, Node *>();
    for(auto it : nodes) this->nodes[it.first] = NodeFactory::create_node(it.second, node_type);
    for(Edge edge : edges) {
        add_edge(edge);
    }
}

centralized::Graph::Graph(const centralized::Graph &other) {
    copyInformation(other.n, other.m, other.nodes);
}

centralized::Graph &centralized::Graph::operator=(const centralized::Graph &other) {
    copyInformation(other.n, other.m, other.nodes);
    return *this;
}

void centralized::Graph::add_edge(Edge edge) {
    edges.insert(edge);
    Node *srcnode = nodes[edge.u];
    Node *destnode = nodes[edge.v];
    srcnode->add_neighbor(destnode);
    destnode->add_neighbor(srcnode);
}

void centralized::Graph::copyInformation(int n, int m, std::map<int, Node *> nodes) {
    this->n = n;
    this->m = m;
    this->nodes = std::map<int, Node *>();
    for(auto it : nodes) this->nodes[it.first] = it.second->dup();
}

bool centralized::Graph::is_connected() {
    std::map<int,bool> visited;
    dfs(nodes.begin()->first, -1, visited);
    for(auto it : nodes) if (!visited[it.first]) return false;
    return true;
}

void centralized::Graph::dfs(int u, int parent, std::map<int,bool> &visited) {
    if (visited[u]) return;
    //std::cout << "\tvisit " << u << '\n';
    visited[u] = true;
    for(auto it : nodes[u]->neighbors) {
        int neighbor_id = it.first;
        if (neighbor_id == parent) continue;
        if (visited[neighbor_id]) continue;
        dfs(neighbor_id, u, visited);
    }
}

void centralized::Graph::print() const {
    std::cout << "Graph::n = " << n << '\n';
    for(auto it : nodes) {
        centralized::Node *node = it.second;
        std::cout << node->to_string() << '\n';
        for(auto it : node->neighbors) {
            centralized::Node *neighbor = it.second;
            std::cout << "\t" << neighbor->to_string() << '\n';
        }
    }
}

centralized::Graph::~Graph() {
    for(auto it : nodes) delete it.second;
}