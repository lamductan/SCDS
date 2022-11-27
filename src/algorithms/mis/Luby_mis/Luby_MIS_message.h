#ifndef SCDS_ALGORITHMS_MIS_LUBY_MIS_LUBY_MIS_MESSAGE_H_
#define SCDS_ALGORITHMS_MIS_LUBY_MIS_LUBY_MIS_MESSAGE_H_

#include "algorithms/ialg_node.h"
#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

class LubyMISMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return LUBY_MIS_MESSAGE; }

    int sender_id;
    bool marked;
    int degree = -1;
    MISNodeStatus MIS_status;

    void copyInformation(int sender_id, bool marked, int degree,
                         MISNodeStatus MIS_status);

  public:
    LubyMISMessage(const char *name = nullptr, short kind = LUBY_MIS_MESSAGE);
    LubyMISMessage(const LubyMISMessage &other);
    LubyMISMessage &operator=(const LubyMISMessage &other);
    virtual LubyMISMessage *dup() const override
    {
        return new LubyMISMessage(*this);
    }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    bool getMarked() const;
    void setMarked(bool marked);
    int getDegree() const;
    void setDegree(int degree);
    MISNodeStatus getStatus() const;
    void setStatus(MISNodeStatus MIS_status);
};

#endif // SCDS_ALGORITHMS_MIS_LUBY_MIS_LUBY_MIS_MESSAGE_H_