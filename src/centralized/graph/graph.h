#ifndef CENTRALIZED_CDS_GRAPH_GRAPH_H_
#define CENTRALIZED_CDS_GRAPH_GRAPH_H_

#include <map>
#include <set>
#include "centralized/graph/node.h"
#include "centralized/graph/edge.h"

namespace centralized {

class Graph {
private:
    void dfs(int u, int parent, std::map<int,bool> &visited);

public:
    std::map<int, Node *> nodes;
    std::set<Edge> edges;
    int n;
    int m;

    Graph(const char *network_file="../scds/gen_random_networks/20/sparse/000.txt");
    Graph(const Graph &other);
    Graph(std::map<int, Node *> nodes, std::set<Edge> edges, NodeType node_type=CDS_SIMPLE_NODE_TYPE);
    Graph &operator=(const Graph &other);
    virtual Graph *dup() const {return new Graph(*this);}
    
    virtual void copyInformation(int n, int m, std::map<int, Node *> nodes);
    virtual void add_edge(Edge edge);
    virtual bool is_connected();
    virtual void print() const;

    virtual ~Graph();
}; //class Graph

}; //namespace centralized

#endif //CENTRALIZED_CDS_GRAPH_GRAPH_H_