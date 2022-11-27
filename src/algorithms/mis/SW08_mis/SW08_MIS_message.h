#ifndef SCDS_ALGORITHMS_MIS_SW08_MIS_SW08_MIS_MESSAGE_H_
#define SCDS_ALGORITHMS_MIS_SW08_MIS_SW08_MIS_MESSAGE_H_

#include "algorithms/mis/SW08_mis/SW08_MIS.h"
#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

class SW08MISMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return SW08_MIS_MESSAGE; }

    int sender_id;
    int r = -1;
    SW08NodeStatus status;

    void copyInformation(int sender_id, int r, SW08NodeStatus status);

  public:
    SW08MISMessage(const char *name = nullptr, short kind = SW08_MIS_MESSAGE);
    SW08MISMessage(const SW08MISMessage &other);
    SW08MISMessage &operator=(const SW08MISMessage &other);
    virtual SW08MISMessage *dup() const override
    {
        return new SW08MISMessage(*this);
    }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    int getR() const;
    void setR(int degree);
    SW08NodeStatus getStatus() const;
    void setStatus(SW08NodeStatus status);

    virtual ~SW08MISMessage();
};

#endif // SCDS_ALGORITHMS_MIS_SW08_MIS_SW08_MIS_MESSAGE_H_