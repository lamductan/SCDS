#ifndef SCDS_CHECKERS_CHECKER_BFS_H_
#define SCDS_CHECKERS_CHECKER_BFS_H_

#include "centralized/algorithms/WAF02_cds/WAF02_CDS.h"
#include "checkers/ichecker.h"

class BFSChecker : public IChecker
{
  public:
    BFSChecker(Network *network);
    virtual bool check(bool is_final_check = true) const;
    virtual bool check_bfs_structure(bool is_final_check) const;

    std::map<int, centralized::BFSTreeStructure>
    get_bfs_structure_from_network() const;

    static bool is_equal_bfs_structure(
        const std::map<int, centralized::BFSTreeStructure> &lhs,
        std::map<int, centralized::BFSTreeStructure> &rhs);
};

#endif // SCDS_CHECKERS_CHECKER_BFS_H_