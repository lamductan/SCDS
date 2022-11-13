#ifndef SCDS_ALGORITHMS_MIS_LAM23_MIS_LAM23_MIS_MESSAGE_H_
#define SCDS_ALGORITHMS_MIS_LAM23_MIS_LAM23_MIS_MESSAGE_H_

#include <omnetpp.h>
#include "algorithms/mis/Luby_mis/Luby_MIS_message.h"
#include "algorithms/mis/Lam23_mis/Lam23_Part1_Two_RS.h"

using namespace omnetpp;

class LamMISMessage : public LubyMISMessage {
public:
    static MessageType getMessageType() { return LAM_MIS_MESSAGE; }

protected:
    LamTwoRSNodeStatus two_rs_status;
    int color;
    void copyInformation(
        int sender_id, bool marked, int degree, NodeStatus status, LamTwoRSNodeStatus two_rs_status, int color);
    void copyInformation(LamTwoRSNodeStatus two_rs_status, int color);

public:
    LamMISMessage(const char *name=nullptr, short kind=LAM_MIS_MESSAGE);
    LamMISMessage(const LamMISMessage& other);
    LamMISMessage& operator=(const LamMISMessage& other);
    virtual LamMISMessage *dup() const override {return new LamMISMessage(*this);}

    // field getter/setter methods
    LamTwoRSNodeStatus getTwoRSStatus() const;
    void setTwoRSStatus(LamTwoRSNodeStatus two_rs_status);
    int getColor() const;
    void setColor(int color);
};

#endif //SCDS_ALGORITHMS_MIS_LAM23_MIS_LAM23_MIS_MESSAGE_H_