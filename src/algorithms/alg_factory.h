#ifndef SCDS_ALGORITHMS_ALG_FACTORY_H_
#define SCDS_ALGORITHMS_ALG_FACTORY_H_

#include <omnetpp.h>

#include "common.h"
#include "algorithms/algs.h"

class AlgFactory
{
public:
    static IAlg *create_alg(IModule *module, AlgType alg_type) {
        if (alg_type == DUMMY_ALG) {
            if (module->getModuleType() == NETWORK_TYPE) return new DummyAlgNetwork(dynamic_cast<Network *>(module));
            else return new DummyAlgNode(dynamic_cast<Node *>(module));
        }
        return nullptr;
    }
};

#endif //SCDS_ALGORITHMS_ALG_FACTORY_H_