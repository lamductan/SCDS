#include "centralized/graph/node.h"
#include "utils/utils.h"
#include <iostream>
#include <string.h>

centralized::Node::Node() {}

centralized::Node::Node(int x, int y, int id)
{
    this->x = x;
    this->y = y;
    this->id = id;
}

centralized::Node::Node(const centralized::Node &other)
{
    copyInformation(other.x, other.y, other.id, other.neighbors);
}

centralized::Node &centralized::Node::operator=(const centralized::Node &other)
{
    copyInformation(other.x, other.y, other.id, other.neighbors);
    return *this;
}

centralized::NodeType centralized::Node::get_node_type()
{
    return CDS_DEFAULT_NODE_TYPE;
}

void centralized::Node::copyInformation(int x, int y, int id)
{
    this->x = x;
    this->y = y;
    this->id = id;
}

void centralized::Node::copyInformation(int x, int y, int id,
                                        std::map<int, Node *> neighbors)
{
    copyInformation(x, y, id);
    this->neighbors = neighbors;
    std::vector<int> neighbors_vector = getMapKeys<int>(this->neighbors);
    this->neighbors_set =
        std::set<int>(neighbors_vector.begin(), neighbors_vector.end());
}

void centralized::Node::add_neighbor(centralized::Node *neighbor)
{
    neighbors[neighbor->id] = neighbor;
    neighbors_set.insert(neighbor->id);
}

bool centralized::Node::is_mis_status_decided() const
{
    return (MIS_status != MIS_UNDECIDED);
}

bool centralized::Node::is_cds_status_decided() const
{
    return (CDS_status != CDS_UNDECIDED);
}

std::string centralized::Node::to_string(int log_level,
                                         bool log_level_on_first_line) const
{
    char buffer[1000];
    std::string tab = std::string(log_level * 4, ' ');
    snprintf(buffer, 1000, "%snode(%04d, %p)", tab.c_str(), id, this);
    return std::string(buffer);
}

centralized::Node::~Node() {}