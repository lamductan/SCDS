#ifndef SCDS_MESSAGES_NEIGHBOR_DISCOVERY_MESSAGE_H_
#define SCDS_MESSAGES_NEIGHBOR_DISCOVERY_MESSAGE_H_

#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

class NeighborDiscoveryMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return NEIGHBOR_DISCOVERY_MESSAGE; }

  protected:
    int sender_id;
    void copyInformation(int sender_id);

  public:
    NeighborDiscoveryMessage(const char *name = nullptr,
                             short kind = NEIGHBOR_DISCOVERY_MESSAGE);
    NeighborDiscoveryMessage(const NeighborDiscoveryMessage &other);
    NeighborDiscoveryMessage &operator=(const NeighborDiscoveryMessage &other);
    virtual NeighborDiscoveryMessage *dup() const override
    {
        return new NeighborDiscoveryMessage(*this);
    }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
};

#endif // SCDS_MESSAGES_NEIGHBOR_DISCOVERY_MESSAGE_H_