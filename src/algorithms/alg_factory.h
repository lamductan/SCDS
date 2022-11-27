#ifndef SCDS_ALGORITHMS_ALG_FACTORY_H_
#define SCDS_ALGORITHMS_ALG_FACTORY_H_

#include "algorithms/algs.h"

class AlgFactory
{
  public:
    static IAlgNode *create_alg(Node *node, const char *alg_name,
                                int starting_round = 1,
                                double threshold_ratio = 1.0);
};

#endif // SCDS_ALGORITHMS_ALG_FACTORY_H_