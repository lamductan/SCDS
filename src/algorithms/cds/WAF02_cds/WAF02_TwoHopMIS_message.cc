#include "algorithms/cds/WAF02_cds/WAF02_TwoHopMIS_message.h"

WAF02TwoHopMISMessage::WAF02TwoHopMISMessage(const char *name, short kind)
    : cMessage(name, kind)
{
}

void WAF02TwoHopMISMessage::copyInformation(
    int sender_id, int level,
    WAF02TwoHopMISMessageType WAF02_two_hop_MIS_message_type)
{
    this->sender_id = sender_id;
    this->WAF02_two_hop_MIS_message_type = WAF02_two_hop_MIS_message_type;
    this->level = level;
}

WAF02TwoHopMISMessage::WAF02TwoHopMISMessage(const WAF02TwoHopMISMessage &other)
    : cMessage((cMessage)other)
{
    copyInformation(other.sender_id, other.level,
                    other.WAF02_two_hop_MIS_message_type);
}

WAF02TwoHopMISMessage &
WAF02TwoHopMISMessage::operator=(const WAF02TwoHopMISMessage &other)
{
    cMessage::operator=(other);
    copyInformation(other.sender_id, other.level,
                    other.WAF02_two_hop_MIS_message_type);
    return *this;
}

int WAF02TwoHopMISMessage::getSenderId() const { return sender_id; }

void WAF02TwoHopMISMessage::setSenderId(int sender_id)
{
    this->sender_id = sender_id;
}

WAF02TwoHopMISMessageType
WAF02TwoHopMISMessage::getWAF02TwoHopMISMessageType() const
{
    return WAF02_two_hop_MIS_message_type;
}

void WAF02TwoHopMISMessage::setWAF02TwoHopMISMessageType(
    WAF02TwoHopMISMessageType WAF02_two_hop_MIS_message_type)
{
    this->WAF02_two_hop_MIS_message_type = WAF02_two_hop_MIS_message_type;
}

int WAF02TwoHopMISMessage::getLevel() const { return level; }

void WAF02TwoHopMISMessage::setLevel(int level) { this->level = level; }

std::string WAF02TwoHopMISMessage::to_string(int log_level) const
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "WAF02_two_hop_MIS_message = {\n";
    s += (tab + one_tab + "WAF02_two_hop_MIS_message_type = " +
          WAF02TwoHopMISMessageType_to_string(WAF02_two_hop_MIS_message_type) +
          ",\n");
    s += (tab + one_tab + "sender_id        = " + std::to_string(sender_id) +
          ",\n");
    s +=
        (tab + one_tab + "level            = " + std::to_string(level) + ",\n");
    return (s += tab + "}");
}

std::string WAF02TwoHopMISMessageType_to_string(
    WAF02TwoHopMISMessageType WAF02_two_hop_MIS_message_type)
{
    switch (WAF02_two_hop_MIS_message_type) {
    case WAF02_TWO_HOP_MIS_MESSAGE_BLACK:
        return "WAF02_TWO_HOP_MIS_MESSAGE_BLACK";
    case WAF02_TWO_HOP_MIS_MESSAGE_GRAY:
        return "WAF02_TWO_HOP_MIS_MESSAGE_GRAY";
    case WAF02_TWO_HOP_MIS_MESSAGE_MARK_COMPLETE:
        return "WAF02_TWO_HOP_MIS_MESSAGE_MARK_COMPLETE";
    default:
        return "";
    }
}