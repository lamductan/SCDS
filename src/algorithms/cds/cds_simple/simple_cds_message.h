#ifndef SCDS_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_CDS_MESSAGE_H_
#define SCDS_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_CDS_MESSAGE_H_

#include "algorithms/ialg_node.h"
#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

class SimpleCDSMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return SIMPLE_CDS_MESSAGE; }

    int sender_id;
    std::map<int, std::array<int, 3>> MIS_info;
    std::set<int> cds_nodes;
    std::map<std::tuple<int, int, int>, std::array<int, 4>>
        nodes_on_directed_paths;

    void copyInformation(
        int sender_id, const std::map<int, std::array<int, 3>> &MIS_info,
        const std::set<int> &cds_nodes,
        const std::map<std::tuple<int, int, int>, std::array<int, 4>>
            &nodes_on_directed_paths);

    std::string to_string(int log_level = 0) const;

  public:
    SimpleCDSMessage(const char *name = nullptr,
                     short kind = SIMPLE_CDS_MESSAGE);
    SimpleCDSMessage(const SimpleCDSMessage &other);
    SimpleCDSMessage &operator=(const SimpleCDSMessage &other);
    virtual SimpleCDSMessage *dup() const override
    {
        return new SimpleCDSMessage(*this);
    }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    std::map<int, std::array<int, 3>> getMISInfo() const;
    void setMISInfo(const std::map<int, std::array<int, 3>> &MIS_info);
    std::set<int> getCDSNodes() const;
    void setCDSNodes(const std::set<int> &cds_nodes);

    std::map<std::tuple<int, int, int>, std::array<int, 4>>
    getNodesOnDirectedPaths() const;

    void setNodesOnDirectedPaths(
        const std::map<std::tuple<int, int, int>, std::array<int, 4>>
            &nodes_on_directed_paths);
};

#endif // SCDS_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_CDS_MESSAGE_H_