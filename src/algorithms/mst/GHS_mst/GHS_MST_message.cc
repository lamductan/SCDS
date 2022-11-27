#include "algorithms/mst/GHS_mst/GHS_MST_message.h"
#include "utils/utils.h"

GHSMSTMessage::GHSMSTMessage(const char *name, short kind)
    : cMessage(name, kind)
{
}

void GHSMSTMessage::copyInformation(
    int sender_id, int receiver_id, const std::tuple<int, int, int> &edge_id,
    int sent_round_id,
    const std::pair<std::tuple<int, int, int>, int> &fragment_core, int level,
    GHSMSTNodeState GHS_MST_node_state, GHSMSTMessageType GHS_MST_message_type,
    const std::tuple<int, int, int> &best_edge_id, int fragment_id,
    int real_sender_id, int real_receiver_id, int message_id)
{
    this->sender_id = sender_id;
    this->receiver_id = receiver_id;
    this->edge_id = edge_id;
    this->sent_round_id = sent_round_id;
    this->fragment_core = fragment_core;
    this->level = level;
    this->GHS_MST_node_state = GHS_MST_node_state;
    this->GHS_MST_message_type = GHS_MST_message_type;
    this->best_edge_id = best_edge_id;
    this->fragment_id = fragment_id;
    this->real_sender_id = real_sender_id;
    this->real_receiver_id = real_receiver_id;
    this->message_id = message_id;
}

GHSMSTMessage::GHSMSTMessage(const GHSMSTMessage &other)
    : cMessage((cMessage)other)
{
    copyInformation(other.sender_id, other.receiver_id, other.edge_id,
                    other.sent_round_id, other.fragment_core, other.level,
                    other.GHS_MST_node_state, other.GHS_MST_message_type,
                    other.best_edge_id, other.fragment_id, other.real_sender_id,
                    other.real_receiver_id, other.message_id);
}

GHSMSTMessage &GHSMSTMessage::operator=(const GHSMSTMessage &other)
{
    cMessage::operator=(other);
    copyInformation(other.sender_id, other.receiver_id, other.edge_id,
                    other.sent_round_id, other.fragment_core, other.level,
                    other.GHS_MST_node_state, other.GHS_MST_message_type,
                    other.best_edge_id, other.fragment_id, other.real_sender_id,
                    other.real_receiver_id, other.message_id);
    return *this;
}

int GHSMSTMessage::getSenderId() const { return sender_id; }

void GHSMSTMessage::setSenderId(int sender_id) { this->sender_id = sender_id; }

int GHSMSTMessage::getReceiverId() const { return receiver_id; }

void GHSMSTMessage::setReceiverId(int receiver_id)
{
    this->receiver_id = receiver_id;
}

std::tuple<int, int, int> GHSMSTMessage::getEdgeId() const { return edge_id; }

void GHSMSTMessage::setEdgeId(const std::tuple<int, int, int> &edge_id)
{
    this->edge_id = edge_id;
}

int GHSMSTMessage::getSentRoundId() const { return sent_round_id; }

void GHSMSTMessage::setSentRoundId(int sent_round_id)
{
    this->sent_round_id = sent_round_id;
}

std::pair<std::tuple<int, int, int>, int> GHSMSTMessage::getFragmentCore() const
{
    return fragment_core;
}

void GHSMSTMessage::setFragmentCore(
    std::pair<std::tuple<int, int, int>, int> fragment_core)
{
    this->fragment_core = fragment_core;
}

int GHSMSTMessage::getLevel() const { return level; }

void GHSMSTMessage::setLevel(int level) { this->level = level; }

GHSMSTNodeState GHSMSTMessage::getGHSMSTNodeState() const
{
    return GHS_MST_node_state;
}

void GHSMSTMessage::setGHSMSTNodeState(GHSMSTNodeState GHS_MST_node_state)
{
    this->GHS_MST_node_state = GHS_MST_node_state;
}

GHSMSTMessageType GHSMSTMessage::getGHSMSTMessageType() const
{
    return GHS_MST_message_type;
}

void GHSMSTMessage::setGHSMSTMessageType(GHSMSTMessageType GHS_MST_message_type)
{
    this->GHS_MST_message_type = GHS_MST_message_type;
}

std::tuple<int, int, int> GHSMSTMessage::getBestEdgeId() const
{
    return best_edge_id;
}

void GHSMSTMessage::setBestEdgeId(const std::tuple<int, int, int> &best_edge_id)
{
    this->best_edge_id = best_edge_id;
}

int GHSMSTMessage::getFragmentId() const { return fragment_id; }

void GHSMSTMessage::setFragmentId(int fragment_id)
{
    this->fragment_id = fragment_id;
}

int GHSMSTMessage::getRealSenderId() const { return real_sender_id; }

void GHSMSTMessage::setRealSenderId(int real_sender_id)
{
    this->real_sender_id = real_sender_id;
}

int GHSMSTMessage::getRealReceiverId() const { return real_receiver_id; }

void GHSMSTMessage::setRealReceiverId(int real_receiver_id)
{
    this->real_receiver_id = real_receiver_id;
}

int GHSMSTMessage::getMessageId() const { return message_id; }

void GHSMSTMessage::setMessageId(int message_id)
{
    this->message_id = message_id;
}

std::string GHSMSTMessage::to_string(int log_level) const
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "GHS_MST_message = {\n";
    s += (tab + one_tab + "GHS_MST_message_type = " +
          GHSMSTMessageType_to_string(GHS_MST_message_type) + ",\n");
    s += (tab + one_tab +
          "sender_id            = " + std::to_string(sender_id) + ",\n");
    s += (tab + one_tab +
          "receiver_id          = " + std::to_string(receiver_id) + ",\n");
    s += (tab + one_tab +
          "real_sender_id       = " + std::to_string(real_sender_id) + ",\n");
    s += (tab + one_tab +
          "real_receiver_id     = " + std::to_string(real_receiver_id) + ",\n");
    s += (tab + one_tab +
          "message_id           = " + std::to_string(message_id) + ",\n");
    s += (tab + one_tab +
          "fragment_id          = " + std::to_string(fragment_id) + ",\n");
    s += (tab + one_tab + "edge_id              = " + tuple_to_string(edge_id) +
          ",\n");
    s += (tab + one_tab +
          "sent_round_id        = " + std::to_string(sent_round_id) + ",\n");
    s += (tab + one_tab + "fragment_core        = {" +
          tuple_to_string(fragment_core.first) + "," +
          std::to_string(fragment_core.second) + "},\n");
    s += (tab + one_tab + "level                = " + std::to_string(level) +
          ",\n");
    s += (tab + one_tab + "GHS_MST_node_state   = " +
          GHSMSTNodeState_to_string(GHS_MST_node_state) + ",\n");
    s += (tab + one_tab +
          "best_edge_id         = " + tuple_to_string(best_edge_id) + "\n");
    return (s += tab + "}");
}

std::string GHSMSTMessageType_to_string(GHSMSTMessageType GHS_MST_message_type)
{
    switch (GHS_MST_message_type) {
    case GHS_MST_MESSAGE_NIL:
        return "GHS_MST_MESSAGE_NIL";
    case GHS_MST_MESSAGE_CONNECT:
        return "GHS_MST_MESSAGE_CONNECT";
    case GHS_MST_MESSAGE_INITIATE:
        return "GHS_MST_MESSAGE_INITIATE";
    case GHS_MST_MESSAGE_TEST:
        return "GHS_MST_MESSAGE_TEST";
    case GHS_MST_MESSAGE_ACCEPT:
        return "GHS_MST_MESSAGE_ACCEPT";
    case GHS_MST_MESSAGE_REJECT:
        return "GHS_MST_MESSAGE_REJECT";
    case GHS_MST_MESSAGE_REPORT:
        return "GHS_MST_MESSAGE_REPORT";
    case GHS_MST_MESSAGE_CHANGE_ROOT:
        return "GHS_MST_MESSAGE_CHANGE_ROOT";
    case GHS_MST_MESSAGE_DONE:
        return "GHS_MST_MESSAGE_DONE";
    default:
        return "";
    }
}
std::string GHSMSTNodeState_to_string(GHSMSTNodeState GHS_MST_node_state)
{
    switch (GHS_MST_node_state) {
    case GHS_MST_NODE_STATE_NIL:
        return "GHS_MST_NODE_STATE_NIL";
    case GHS_MST_NODE_STATE_FOUND:
        return "GHS_MST_NODE_STATE_FOUND";
    case GHS_MST_NODE_STATE_FIND:
        return "GHS_MST_NODE_STATE_FIND";
    case GHS_MST_NODE_STATE_DONE:
        return "GHS_MST_NODE_STATE_DONE";
    default:
        return "";
    }
}

std::string GHSMSTEdgeState_to_string(GHSMSTEdgeState GHS_MST_edge_state)
{
    switch (GHS_MST_edge_state) {
    case GHS_MST_EDGE_STATE_NIL:
        return "GHS_MST_EDGE_STATE_NIL";
    case GHS_MST_EDGE_STATE_BASIC:
        return "GHS_MST_EDGE_STATE_BASIC";
    case GHS_MST_EDGE_STATE_BRANCH:
        return "GHS_MST_EDGE_STATE_BRANCH";
    case GHS_MST_EDGE_STATE_REJECTED:
        return "GHS_MST_EDGE_STATE_REJECTED";
    default:
        return "";
    }
}