#ifndef SCDS_ALGORITHMS_IALG_H_
#define SCDS_ALGORITHMS_IALG_H_

#include <omnetpp.h>

#include "common.h"

using namespace omnetpp;

class IAlg
{
public:
    virtual void start() = 0;
    virtual void handle_message(cMessage *msg) = 0;
    virtual ~IAlg() {}
};

#endif //SCDS_ALGORITHMS_IALG_H_