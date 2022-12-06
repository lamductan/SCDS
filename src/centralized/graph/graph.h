#ifndef CENTRALIZED_CDS_GRAPH_GRAPH_H_
#define CENTRALIZED_CDS_GRAPH_GRAPH_H_

#include "centralized/graph/bfs_tree_structure.h"
#include "centralized/graph/edge.h"
#include "centralized/graph/node.h"
#include <map>
#include <set>
#include <vector>

namespace centralized {

class Graph
{
  private:
    void dfs(int u, int parent, std::map<int, bool> &visited);

  public:
    std::map<int, Node *> nodes;
    std::set<Edge> edges;
    int n;
    int m;
    int max_node_id = -1;

    Graph(const char *network_file =
              "../scds/gen_random_networks/20/sparse/000.txt");
    Graph(const Graph &other);
    Graph(const std::map<int, Node *> &nodes, const std::set<Edge> &edges,
          NodeType node_type = CDS_SIMPLE_NODE_TYPE);
    Graph(const std::set<int> &nodes,
          const std::set<std::tuple<int, int, int>> &edges);

    Graph &operator=(const Graph &other);
    virtual Graph *dup() const { return new Graph(*this); }

    virtual void copyInformation(int n, int m, std::map<int, Node *> nodes);
    virtual void add_edge(Edge edge);
    void add_edge(const std::tuple<int, int, int> &edge_id);
    virtual bool is_connected();
    virtual void print() const;

    int compute_total_weight() const;
    std::vector<centralized::Edge> build_mst() const;

    void find_two_hop_neighbors_of_a_node(Node *node);
    void find_two_hop_neighbors();
    void find_three_hop_neighbors_of_a_node(Node *node);
    void find_three_hop_neighbors();

    std::map<int, BFSTreeStructure> find_bfs_tree_structure_from_root(int root);
    static int
    get_max_level(const std::map<int, BFSTreeStructure> &bfs_tree_structures);

    virtual ~Graph();
}; // class Graph

}; // namespace centralized

#endif // CENTRALIZED_CDS_GRAPH_GRAPH_H_