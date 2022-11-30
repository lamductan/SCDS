#ifndef SCDS_ALGORITHMS_WAF02_CDS_WAF02_TWO_HOP_MIS_MESSAGE_H_
#define SCDS_ALGORITHMS_WAF02_CDS_WAF02_TWO_HOP_MIS_MESSAGE_H_

#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

enum WAF02TwoHopMISMessageType
{
    WAF02_TWO_HOP_MIS_MESSAGE_BLACK,
    WAF02_TWO_HOP_MIS_MESSAGE_GRAY,
    WAF02_TWO_HOP_MIS_MESSAGE_MARK_COMPLETE
};

class WAF02TwoHopMISMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return WAF02_TWO_HOP_MIS_MESSAGE; }

    int sender_id = -1;
    int level = -1;
    WAF02TwoHopMISMessageType WAF02_two_hop_MIS_message_type;

    void
    copyInformation(int sender_id, int level,
                    WAF02TwoHopMISMessageType WAF02_two_hop_MIS_message_type);

  public:
    WAF02TwoHopMISMessage(const char *name = nullptr,
                          short kind = WAF02_TWO_HOP_MIS_MESSAGE);
    WAF02TwoHopMISMessage(const WAF02TwoHopMISMessage &other);
    WAF02TwoHopMISMessage &operator=(const WAF02TwoHopMISMessage &other);
    virtual WAF02TwoHopMISMessage *dup() const override
    {
        return new WAF02TwoHopMISMessage(*this);
    }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    WAF02TwoHopMISMessageType getWAF02TwoHopMISMessageType() const;
    void setWAF02TwoHopMISMessageType(
        WAF02TwoHopMISMessageType WAF02_two_hop_MIS_message_type);
    int getLevel() const;
    void setLevel(int level);
    std::string to_string(int log_level = 0) const;
};

std::string WAF02TwoHopMISMessageType_to_string(
    WAF02TwoHopMISMessageType WAF02_two_hop_MIS_message_type);

#endif // SCDS_ALGORITHMS_WAF02_CDS_WAF02_TWO_HOP_MIS_MESSAGE_H_