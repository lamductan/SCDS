#ifndef SCDS_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_CDS_MESSAGE_H_
#define SCDS_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_CDS_MESSAGE_H_

#include <omnetpp.h>
#include "messages/message_types.h"
#include "algorithms/ialg_node.h"

using namespace omnetpp;

class SimpleCDSMessage : public cMessage {
public:
    static MessageType getMessageType() { return SIMPLE_CDS_MESSAGE; }

protected:
    int sender_id;
    std::map<int, std::array<int,3>> MIS_info;
    std::set<int> cds_nodes;

    void copyInformation(
        int sender_id, const std::map<int, std::array<int,3>> &MIS_info, const std::set<int> &cds_nodes);

public:
    SimpleCDSMessage(const char *name=nullptr, short kind=SIMPLE_CDS_MESSAGE);
    SimpleCDSMessage(const SimpleCDSMessage& other);
    SimpleCDSMessage& operator=(const SimpleCDSMessage& other);
    virtual SimpleCDSMessage *dup() const override {return new SimpleCDSMessage(*this);}

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    std::map<int, std::array<int,3>> getMISInfo() const;
    void setMISInfo(const std::map<int, std::array<int,3>> &MIS_info);
    std::set<int> getCDSNodes() const;
    void setCDSNodes(const std::set<int> &cds_nodes);
    std::string to_string(int n_tab=0) const;
};

#endif //SCDS_ALGORITHMS_CDS_SIMPLE_CDS_SIMPLE_CDS_MESSAGE_H_