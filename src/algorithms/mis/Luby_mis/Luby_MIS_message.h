#ifndef SCDS_ALGORITHMS_MIS_LUBY_MIS_LUBY_MIS_MESSAGE_H_
#define SCDS_ALGORITHMS_MIS_LUBY_MIS_LUBY_MIS_MESSAGE_H_

#include <omnetpp.h>
#include "messages/message_types.h"
#include "algorithms/ialg_node.h"

using namespace omnetpp;

class LubyMISMessage : public cMessage {
public:
    static MessageType getMessageType() { return LUBY_MIS_MESSAGE; }

protected:
    int sender_id;
    bool marked;
    int degree = -1;
    NodeStatus status;

    void copyInformation(int sender_id, bool marked, int degree, NodeStatus status);

public:
    LubyMISMessage(const char *name=nullptr, short kind=LUBY_MIS_MESSAGE);
    LubyMISMessage(const LubyMISMessage& other);
    LubyMISMessage& operator=(const LubyMISMessage& other);
    virtual LubyMISMessage *dup() const override {return new LubyMISMessage(*this);}

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    bool getMarked() const;
    void setMarked(bool marked);
    int getDegree() const;
    void setDegree(int degree);
    NodeStatus getStatus() const;
    void setStatus(NodeStatus status);
};

#endif //SCDS_ALGORITHMS_MIS_LUBY_MIS_LUBY_MIS_MESSAGE_H_