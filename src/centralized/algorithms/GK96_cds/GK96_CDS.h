#ifndef CENTRALIZED_CDS_ALGORITHMS_CDS_GK96_CDS_GK96_H_
#define CENTRALIZED_CDS_ALGORITHMS_CDS_GK96_CDS_GK96_H_

#include <vector>
#include "centralized/graph/graph.h"
#include "centralized/algorithms/cds_simple/CDS_simple.h"

namespace centralized {

class GK96CDSAlg : public CDSSimpleAlg {
public:
    GK96CDSAlg();
    GK96CDSAlg(std::map<int, Node *> nodes, std::set<Edge> edges);

    virtual std::set<int> find_cds_nodes_from_mis_nodes(const std::vector<int> &mis_nodes);
    std::vector<Edge> find_cluster_edges(std::set<int> &mis_nodes_set);
    std::vector<Edge> build_mst(std::vector<Edge> &edges);
}; //class GK96CDSAlg

}; //namespace centralized

#endif //CENTRALIZED_CDS_ALGORITHMS_CDS_GK96_CDS_GK96_H_