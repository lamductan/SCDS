#include "centralized/graph/edge.h"
#include <set>
#include <tuple>

centralized::Edge::Edge() {}

centralized::Edge::Edge(int u, int v, int w)
{
    if (u > v) std::swap(u, v);
    this->u = u;
    this->v = v;
    this->w = w;
}

centralized::Edge::Edge(const Edge &other)
{
    copyInformation(other.u, other.v, other.w, other.nodes_on_edge);
}

bool centralized::Edge::operator<(const Edge &rhs) const
{
    if (w < rhs.w) return true;
    if (w == rhs.w && u < rhs.u) return true;
    if (w == rhs.w && u == rhs.u && v < rhs.v) return true;
    return false;
}

void centralized::Edge::copyInformation(int u, int v, int w,
                                        std::vector<int> nodes_on_edge)
{
    if (u > v) std::swap(u, v);
    this->u = u;
    this->v = v;
    this->w = w;
    this->nodes_on_edge = nodes_on_edge;
}

void centralized::Edge::add_node_on_edge(int nodeid)
{
    std::set<int> s(nodes_on_edge.begin(), nodes_on_edge.end());
    if (s.count(nodeid) > 0) return;
    nodes_on_edge.push_back(nodeid);
}

std::tuple<int, int, int> centralized::Edge::get_id() const
{
    if (u > v)
        return { w, v, u };
    else
        return { w, u, v };
}

int centralized::Edge::get_other_endpoint(int endpoint) const
{
    return u + v - endpoint;
}

std::string centralized::Edge::to_string()
{
    std::string s = "Edge[" + std::to_string(u) + "," + std::to_string(v) +
                    "," + std::to_string(w);
    s += ",nodes_on_edges=(";
    for (int id : nodes_on_edge) s += std::to_string(id) + ",";
    s += ")]";
    return s;
}