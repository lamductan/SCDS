#ifndef CENTRALIZED_CDS_GRAPH_EDGE_H_
#define CENTRALIZED_CDS_GRAPH_EDGE_H_

#include <set>
#include <string>

namespace centralized {

class Edge {
public:
    int u;
    int v;
    int w;
    std::set<int> nodes_on_edge;
    bool operator<(const Edge &rhs) const;

    Edge();
    Edge(int u, int v, int w=0);
    Edge(const Edge &other);

    void copyInformation(int u, int v, int w, std::set<int> nodes_on_edge);
    void add_node_on_edge(int nodeid);

    std::string to_string();
}; //class Edge

}; //namespace centralized

#endif //CENTRALIZED_CDS_GRAPH_EDGE_H_