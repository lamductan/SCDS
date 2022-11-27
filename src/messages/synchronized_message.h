#ifndef SCDS_MESSAGES_SYNCHRONIZED_MESSAGE_H_
#define SCDS_MESSAGES_SYNCHRONIZED_MESSAGE_H_

#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

enum SynchronizedMessageType
{
    SYNCHRONIZED_START_ROUND,
    SYNCHRONIZED_FINISH
};

class SynchronizedMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return SYNCHRONIZED_MESSAGE; }

  protected:
    int sender_id;
    int round_id;
    SynchronizedMessageType synchronized_message_type;

    void copyInformation(int sender_id, int round_id,
                         SynchronizedMessageType synchronized_message_type);

  public:
    SynchronizedMessage(const char *name = nullptr,
                        short kind = SYNCHRONIZED_MESSAGE);
    SynchronizedMessage(const SynchronizedMessage &other);
    SynchronizedMessage &operator=(const SynchronizedMessage &other);
    virtual SynchronizedMessage *dup() const override
    {
        return new SynchronizedMessage(*this);
    }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    int getRoundId() const;
    void setRoundId(int round_id);
    SynchronizedMessageType getSynchronizedMessageType() const;
    void setSynchronizedMessageType(
        SynchronizedMessageType synchronized_message_type);
};

#endif // SCDS_MESSAGES_SYNCHRONIZED_MESSAGE_H_