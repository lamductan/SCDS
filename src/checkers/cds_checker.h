#ifndef SCDS_CHECKERS_CHECKER_CDS_H_
#define SCDS_CHECKERS_CHECKER_CDS_H_

#include "centralized/graph/graph.h"
#include "checkers/ichecker.h"

class CDSChecker : public IChecker
{
  private:
    std::set<int> selected_nodes_set;
    std::map<int, centralized::Node *> selected_nodes_map;
    centralized::Graph *construct_cds_subgraph() const;

  public:
    CDSChecker(Network *network);
    virtual bool check(bool is_final_check = true) const;
    virtual bool check_connected(bool is_final_check) const;

    virtual ~CDSChecker();
};

#endif // SCDS_CHECKERS_CHECKER_CDS_H_