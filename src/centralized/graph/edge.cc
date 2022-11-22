#include "centralized/graph/edge.h"

centralized::Edge::Edge() {}

centralized::Edge::Edge(int u, int v, int w) {
    if (u < v) std::swap(u, v);
    this->u = u;
    this->v = v;
    this->w = w;
}

centralized::Edge::Edge(const Edge &other) {
    copyInformation(other.u, other.v, other.w, other.nodes_on_edge);
}

bool centralized::Edge::operator<(const Edge &rhs) const {
    if (u < rhs.u) return true;
    if (u == rhs.u && v < rhs.v) return true;
    return false;
}

void centralized::Edge::copyInformation(int u, int v, int w, std::set<int> nodes_on_edge) {
    if (u < v) std::swap(u, v);
    this->u = u;
    this->v = v;
    this->w = w;
    this->nodes_on_edge = nodes_on_edge;
}

void centralized::Edge::add_node_on_edge(int nodeid) {
    nodes_on_edge.insert(nodeid);
}

std::string centralized::Edge::to_string() {
    std::string s = "Edge[" + std::to_string(u) + "," + std::to_string(v) + "," + std::to_string(w);
    s += ",nodes_on_edges=(";
    for(int id : nodes_on_edge) s += std::to_string(id) + ",";
    s += ")]";
    return s;
}