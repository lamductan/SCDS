#ifndef CENTRALIZED_CDS_GRAPH_NODE_H_
#define CENTRALIZED_CDS_GRAPH_NODE_H_

#include <map>
#include <set>
#include <string>

namespace centralized {

enum MISNodeStatus {
    MIS_UNDECIDED,
    IN_MIS,
    NOT_IN_MIS
};

enum CDSNodeStatus {
    CDS_UNDECIDED,
    IN_CDS,
    NOT_IN_CDS
};

enum NodeType {
    CDS_DEFAULT_NODE_TYPE,
    CDS_SIMPLE_NODE_TYPE,
    CDS_GK96_NODE_TYPE,
    CDS_WAF02_NODE_TYPE
};

class Node {
public:
    int id;
    int x;
    int y;
    std::map<int, Node *> neighbors;
    std::set<int> neighbors_set;
    std::map<int, Node *> all_remained_neighbors;

    MISNodeStatus MIS_status = MIS_UNDECIDED;
    CDSNodeStatus CDS_status = CDS_UNDECIDED;
    int dominator = -1;

    std::map<Node *, std::array<Node *, 2>> two_hop_neighbors;

    Node();
    Node(int x, int y, int id);
    Node(const Node &other);
    Node &operator=(const Node &other);
    virtual Node *dup() const {return new Node(*this);}

    virtual void copyInformation(int x, int y, int id);
    virtual void copyInformation(int x, int y, int id, std::map<int, Node *> neighbors);
    virtual void add_neighbor(Node *neighbor);
    virtual NodeType get_node_type();

    bool is_mis_status_decided() const;
    bool is_cds_status_decided() const;

    std::string to_string() const;
    virtual ~Node();
}; //class Node

}; //namespace centralized

#endif //CENTRALIZED_CDS_GRAPH_NODE_H_