#include "algorithms/mis/SW08_mis/SW08_MIS_message.h"

SW08MISMessage::SW08MISMessage(const char *name, short kind)
    : cMessage(name, kind)
{
}

void SW08MISMessage::copyInformation(int sender_id, int r,
                                     SW08NodeStatus status)
{
    this->sender_id = sender_id;
    this->r = r;
    this->status = status;
}

SW08MISMessage::SW08MISMessage(const SW08MISMessage &other)
    : cMessage((cMessage)other)
{
    copyInformation(other.sender_id, other.r, other.status);
}

SW08MISMessage &SW08MISMessage::operator=(const SW08MISMessage &other)
{
    cMessage::operator=(other);
    copyInformation(other.sender_id, other.r, other.status);
    return *this;
}

int SW08MISMessage::getSenderId() const { return sender_id; }

void SW08MISMessage::setSenderId(int sender_id) { this->sender_id = sender_id; }

int SW08MISMessage::getR() const { return r; }

void SW08MISMessage::setR(int r) { this->r = r; }

SW08NodeStatus SW08MISMessage::getStatus() const { return status; }

void SW08MISMessage::setStatus(SW08NodeStatus status) { this->status = status; }

SW08MISMessage::~SW08MISMessage() {}
