#ifndef SCDS_ALGORITHMS_ALG_FACTORY_H_
#define SCDS_ALGORITHMS_ALG_FACTORY_H_

#include <omnetpp.h>

#include "common.h"
#include "algorithms/algs.h"

class AlgFactory
{
public:
    static IAlgNode *create_alg(Node *node, const char* alg_name, int starting_round=1) {
        if (strcmp(alg_name, "dummy") == 0) {
            EV << "Create DummyAlg instance\n";
            return new DummyAlg(node, starting_round);
        } else if (strcmp(alg_name, "MIS-Luby") == 0) {
            EV << "Create LubyMISAlg instance\n";
            return new LubyMISAlg(node, starting_round);
        } else if (strcmp(alg_name, "MIS-KW08") == 0) {
            EV << "Create KW08MISAlg instance\n";
            return new KW08MISAlg(node, starting_round);
        }
        return nullptr;
    }
};

#endif //SCDS_ALGORITHMS_ALG_FACTORY_H_