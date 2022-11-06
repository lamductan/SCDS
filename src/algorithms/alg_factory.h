#ifndef SCDS_ALGORITHMS_ALG_FACTORY_H_
#define SCDS_ALGORITHMS_ALG_FACTORY_H_

#include <omnetpp.h>

#include "common.h"
#include "algorithms/algs.h"

class AlgFactory
{
public:
    static IAlgNode *create_alg(Node *node, const char* alg_name) {
        if (strcmp(alg_name, "dummy") == 0) {
            return new DummyAlg(node, alg_name);
        } else {
            EV << "Algorithm: " << alg_name << '\n';
            return new DummyAlg(node, alg_name);
        }
        return nullptr;
    }
};

#endif //SCDS_ALGORITHMS_ALG_FACTORY_H_