#ifndef SCDS_COMMON_H_
#define SCDS_COMMON_H_

#include <omnetpp.h>

using namespace omnetpp;

enum ModuleType
{
    NODE_TYPE,
    NETWORK_TYPE
};

class IModule : public cSimpleModule
{
  public:
    virtual ModuleType getModuleType() const = 0;
    int current_round_id = 0;
};

#endif // SCDS_COMMON_H_