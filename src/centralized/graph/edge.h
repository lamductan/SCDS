#ifndef CENTRALIZED_CDS_GRAPH_EDGE_H_
#define CENTRALIZED_CDS_GRAPH_EDGE_H_

#include <string>
#include <vector>

namespace centralized {

class Edge
{
  public:
    int u;
    int v;
    int w = 1;
    std::vector<int> nodes_on_edge;
    bool operator<(const Edge &rhs) const;

    Edge();
    Edge(int u, int v, int w = 1);
    Edge(const Edge &other);

    void copyInformation(int u, int v, int w, std::vector<int> nodes_on_edge);
    void add_node_on_edge(int nodeid);
    std::tuple<int, int, int> get_id() const;
    int get_other_endpoint(int endpoint) const;

    std::string to_string();
}; // class Edge

}; // namespace centralized

#endif // CENTRALIZED_CDS_GRAPH_EDGE_H_