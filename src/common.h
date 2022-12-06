#ifndef SCDS_COMMON_H_
#define SCDS_COMMON_H_

#include <omnetpp.h>
#include <tuple>

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

const std::tuple<int, int, int> INF_EDGE_ID = { INT_MAX, -1, -1 };

#endif // SCDS_COMMON_H_