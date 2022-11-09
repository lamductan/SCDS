#ifndef SCDS_ALGORITHMS_MIS_KW08_MIS_KW08_MIS_MESSAGE_H_
#define SCDS_ALGORITHMS_MIS_KW08_MIS_KW08_MIS_MESSAGE_H_

#include <omnetpp.h>
#include "messages/message_types.h"
#include "algorithms/mis/KW08_mis/KW08_MIS.h"

using namespace omnetpp;

class KW08MISMessage : public cMessage {
public:
    static MessageType getMessageType() { return KW08_MIS_MESSAGE; }

protected:
    int sender_id;
    int r = -1;
    KW08NodeStatus status;

    void copyInformation(int sender_id, int r, KW08NodeStatus status);

public:
    KW08MISMessage(const char *name=nullptr, short kind=KW08_MIS_MESSAGE);
    KW08MISMessage(const KW08MISMessage& other);
    KW08MISMessage& operator=(const KW08MISMessage& other);
    virtual KW08MISMessage *dup() const override {return new KW08MISMessage(*this);}

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    int getR() const;
    void setR(int degree);
    KW08NodeStatus getStatus() const;
    void setStatus(KW08NodeStatus status);

    virtual ~KW08MISMessage();
};

#endif //SCDS_ALGORITHMS_MIS_KW08_MIS_KW08_MIS_MESSAGE_H_