#ifndef SCDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_CDS_MESSAGE_H_
#define SCDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_CDS_MESSAGE_H_

#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

enum WAF02CDSMessageType
{
    WAF02_CDS_MESSAGE_QUERY,
    WAF02_CDS_MESSAGE_REPORT,
    WAF02_CDS_MESSAGE_ROOT,
    WAF02_CDS_MESSAGE_INVITE1,
    WAF02_CDS_MESSAGE_INVITE2,
    WAF02_CDS_MESSAGE_JOIN
};

class WAF02CDSMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return WAF02_CDS_MESSAGE; }

    int sender_id = -1;
    int n_mis_neighbors = -1;
    WAF02CDSMessageType WAF02_CDS_message_type;

    void copyInformation(int sender_id, int n_mis_neighbors,
                         WAF02CDSMessageType WAF02_CDS_message_type);

  public:
    WAF02CDSMessage(const char *name = nullptr, short kind = WAF02_CDS_MESSAGE);
    WAF02CDSMessage(const WAF02CDSMessage &other);
    WAF02CDSMessage &operator=(const WAF02CDSMessage &other);
    virtual WAF02CDSMessage *dup() const override
    {
        return new WAF02CDSMessage(*this);
    }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    WAF02CDSMessageType getWAF02CDSMessageType() const;
    void setWAF02CDSMessageType(WAF02CDSMessageType WAF02_CDS_message_type);
    int getNMISNeighbors() const;
    void setNMISNeighbors(int n_mis_neighbors);
    std::string to_string(int log_level = 0) const;
};

std::string
WAF02CDSMessageType_to_string(WAF02CDSMessageType WAF02_CDS_message_type);

#endif // SCDS_ALGORITHMS_CDS_WAF02_CDS_WAF02_CDS_MESSAGE_H_