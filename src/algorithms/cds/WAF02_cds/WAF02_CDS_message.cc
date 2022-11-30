#include "algorithms/cds/WAF02_cds/WAF02_CDS_message.h"

WAF02CDSMessage::WAF02CDSMessage(const char *name, short kind)
    : cMessage(name, kind)
{
}

void WAF02CDSMessage::copyInformation(
    int sender_id, int n_mis_neighbors,
    WAF02CDSMessageType WAF02_CDS_message_type)
{
    this->sender_id = sender_id;
    this->WAF02_CDS_message_type = WAF02_CDS_message_type;
    this->n_mis_neighbors = n_mis_neighbors;
}

WAF02CDSMessage::WAF02CDSMessage(const WAF02CDSMessage &other)
    : cMessage((cMessage)other)
{
    copyInformation(other.sender_id, other.n_mis_neighbors,
                    other.WAF02_CDS_message_type);
}

WAF02CDSMessage &WAF02CDSMessage::operator=(const WAF02CDSMessage &other)
{
    cMessage::operator=(other);
    copyInformation(other.sender_id, other.n_mis_neighbors,
                    other.WAF02_CDS_message_type);
    return *this;
}

int WAF02CDSMessage::getSenderId() const { return sender_id; }

void WAF02CDSMessage::setSenderId(int sender_id)
{
    this->sender_id = sender_id;
}

WAF02CDSMessageType WAF02CDSMessage::getWAF02CDSMessageType() const
{
    return WAF02_CDS_message_type;
}

void WAF02CDSMessage::setWAF02CDSMessageType(
    WAF02CDSMessageType WAF02_CDS_message_type)
{
    this->WAF02_CDS_message_type = WAF02_CDS_message_type;
}

int WAF02CDSMessage::getNMISNeighbors() const { return n_mis_neighbors; }

void WAF02CDSMessage::setNMISNeighbors(int n_mis_neighbors)
{
    this->n_mis_neighbors = n_mis_neighbors;
}

std::string WAF02CDSMessage::to_string(int log_level) const
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "WAF02_two_hop_MIS_message = {\n";
    s += (tab + one_tab + "WAF02_CDS_message_type = " +
          WAF02CDSMessageType_to_string(WAF02_CDS_message_type) + ",\n");
    s += (tab + one_tab + "sender_id        = " + std::to_string(sender_id) +
          ",\n");
    s += (tab + one_tab +
          "n_mis_neighbors  = " + std::to_string(n_mis_neighbors) + ",\n");
    return (s += tab + "}");
}

std::string
WAF02CDSMessageType_to_string(WAF02CDSMessageType WAF02_CDS_message_type)
{
    switch (WAF02_CDS_message_type) {
    case WAF02_CDS_MESSAGE_QUERY:
        return "WAF02_CDS_MESSAGE_QUERY";
    case WAF02_CDS_MESSAGE_REPORT:
        return "WAF02_CDS_MESSAGE_REPORT";
    case WAF02_CDS_MESSAGE_ROOT:
        return "WAF02_CDS_MESSAGE_ROOT";
    case WAF02_CDS_MESSAGE_INVITE1:
        return "WAF02_CDS_MESSAGE_INVITE1";
    case WAF02_CDS_MESSAGE_INVITE2:
        return "WAF02_CDS_MESSAGE_INVITE2";
    case WAF02_CDS_MESSAGE_JOIN:
        return "WAF02_CDS_MESSAGE_JOIN";
    default:
        return "";
    }
}