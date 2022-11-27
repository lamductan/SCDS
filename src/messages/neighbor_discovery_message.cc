#include "messages/neighbor_discovery_message.h"

NeighborDiscoveryMessage::NeighborDiscoveryMessage(const char *name, short kind)
    : cMessage(name, kind)
{
}

void NeighborDiscoveryMessage::copyInformation(int sender_id)
{
    this->sender_id = sender_id;
}

NeighborDiscoveryMessage::NeighborDiscoveryMessage(
    const NeighborDiscoveryMessage &other)
    : cMessage((cMessage)other)
{
    copyInformation(other.sender_id);
}

NeighborDiscoveryMessage &
NeighborDiscoveryMessage::operator=(const NeighborDiscoveryMessage &other)
{
    cMessage::operator=(other);
    copyInformation(other.sender_id);
    return *this;
}

int NeighborDiscoveryMessage::getSenderId() const { return sender_id; }

void NeighborDiscoveryMessage::setSenderId(int sender_id)
{
    this->sender_id = sender_id;
}