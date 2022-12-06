#include "algorithms/mst/AMP22_mst/AMP22_mst_message.h"
#include "utils/utils.h"

AMP22MSTMessage::AMP22MSTMessage(const char *name, short kind)
    : cMessage(name, kind)
{
}

void AMP22MSTMessage::copyInformation(
    int sender_id, int receiver_id, int sent_round_id, int fragment_id,
    int level, int parent, AMP22MSTCoinValue coin_value,
    const std::tuple<int, int, int> &moe, int new_fragment_id, int new_level,
    int real_sender_id, int real_receiver_id, bool is_MST_edge,
    AMP22MSTRoundPhaseActionType round_action_type)
{
    this->sender_id = sender_id;
    this->receiver_id = receiver_id;
    this->sent_round_id = sent_round_id;
    this->fragment_id = fragment_id;
    this->level = level;
    this->parent = parent;
    this->coin_value = coin_value;
    this->moe = moe;
    this->new_fragment_id = new_fragment_id;
    this->new_level = new_level;
    this->real_sender_id = real_sender_id;
    this->real_receiver_id = real_receiver_id;
    this->is_MST_edge = is_MST_edge;
    this->round_action_type = round_action_type;
}

AMP22MSTMessage::AMP22MSTMessage(const AMP22MSTMessage &other)
    : cMessage((cMessage)other)
{
    copyInformation(
        other.sender_id, other.receiver_id, other.sent_round_id,
        other.fragment_id, other.level, other.parent, other.coin_value,
        other.moe, other.new_fragment_id, other.new_level, other.real_sender_id,
        other.real_receiver_id, other.is_MST_edge, other.round_action_type);
}

AMP22MSTMessage &AMP22MSTMessage::operator=(const AMP22MSTMessage &other)
{
    cMessage::operator=(other);
    copyInformation(
        other.sender_id, other.receiver_id, other.sent_round_id,
        other.fragment_id, other.level, other.parent, other.coin_value,
        other.moe, other.new_fragment_id, other.new_level, other.real_sender_id,
        other.real_receiver_id, other.is_MST_edge, other.round_action_type);
    return *this;
}

int AMP22MSTMessage::getSenderId() const { return sender_id; }

void AMP22MSTMessage::setSenderId(int sender_id)
{
    this->sender_id = sender_id;
}

int AMP22MSTMessage::getReceiverId() const { return receiver_id; }

void AMP22MSTMessage::setReceiverId(int receiver_id)
{
    this->receiver_id = receiver_id;
}

int AMP22MSTMessage::getSentRoundId() const { return sent_round_id; }

void AMP22MSTMessage::setSentRoundId(int sent_round_id)
{
    this->sent_round_id = sent_round_id;
}

int AMP22MSTMessage::getFragmentId() const { return fragment_id; }

void AMP22MSTMessage::setFragmentId(int fragment_id)
{
    this->fragment_id = fragment_id;
}

int AMP22MSTMessage::getLevel() const { return level; }

void AMP22MSTMessage::setLevel(int level) { this->level = level; }

int AMP22MSTMessage::getParent() const { return parent; }

void AMP22MSTMessage::setParent(int parent) { this->parent = parent; }

AMP22MSTCoinValue AMP22MSTMessage::getCoinValue() const { return coin_value; }

void AMP22MSTMessage::setCoinValue(AMP22MSTCoinValue coin_value)
{
    this->coin_value = coin_value;
}

std::tuple<int, int, int> AMP22MSTMessage::getMOE() const { return moe; }

void AMP22MSTMessage::setMOE(const std::tuple<int, int, int> &moe)
{
    this->moe = moe;
}

int AMP22MSTMessage::getNewFragmentId() const { return new_fragment_id; }

void AMP22MSTMessage::setNewFragmentId(int new_fragment_id)
{
    this->new_fragment_id = new_fragment_id;
}

int AMP22MSTMessage::getNewLevel() const { return new_level; }

void AMP22MSTMessage::setNewLevel(int new_level)
{
    this->new_level = new_level;
}

int AMP22MSTMessage::getRealSenderId() const { return real_sender_id; }

void AMP22MSTMessage::setRealSenderId(int real_sender_id)
{
    this->real_sender_id = real_sender_id;
}

int AMP22MSTMessage::getRealReceiverId() const { return real_receiver_id; }

void AMP22MSTMessage::setRealReceiverId(int real_receiver_id)
{
    this->real_receiver_id = real_receiver_id;
}

bool AMP22MSTMessage::getIsMSTEdge() const { return is_MST_edge; }

void AMP22MSTMessage::setIsMSTEdge(bool is_MST_edge)
{
    this->is_MST_edge = is_MST_edge;
}

AMP22MSTRoundPhaseActionType AMP22MSTMessage::getRoundActionType() const
{
    return round_action_type;
}

void AMP22MSTMessage::setRoundActionType(
    AMP22MSTRoundPhaseActionType round_action_type)
{
    this->round_action_type = round_action_type;
}

std::string AMP22MSTMessage::to_string(int log_level) const
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "AMP22_MST_message = {\n";
    s += (tab + one_tab + "sender_id         = " + std::to_string(sender_id) +
          ",\n");
    s += (tab + one_tab + "receiver_id       = " + std::to_string(receiver_id) +
          ",\n");
    s += (tab + one_tab + "round_action_type = " +
          AMP22MSTRoundPhaseActionType_to_string(round_action_type) + ",\n");
    s += (tab + one_tab +
          "sent_round_id     = " + std::to_string(sent_round_id) + ",\n");
    s += (tab + one_tab + "fragment_id       = " + std::to_string(fragment_id) +
          ",\n");
    s += (tab + one_tab + "level             = " + std::to_string(level) +
          ",\n");
    s += (tab + one_tab + "parent            = " + std::to_string(parent) +
          ",\n");
    s += (tab + one_tab + "coin_value        = " +
          AMP22MSTCoinValue_to_string(coin_value) + ",\n");
    s += (tab + one_tab + "moe               = " + tuple_to_string(moe) + "\n");
    s += (tab + one_tab +
          "new_fragment_id   = " + std::to_string(new_fragment_id) + ",\n");
    s += (tab + one_tab + "new_level         = " + std::to_string(new_level) +
          ",\n");
    s += (tab + one_tab +
          "real_sender_id    = " + std::to_string(real_sender_id) + ",\n");
    s += (tab + one_tab +
          "real_receiver_id  = " + std::to_string(real_receiver_id) + ",\n");
    s += (tab + one_tab + "is_MST_edge       = " + std::to_string(is_MST_edge) +
          ",\n");
    return (s += tab + "}");
}

std::string AMP22MSTCoinValue_to_string(AMP22MSTCoinValue coin_value)
{
    switch (coin_value) {
    case AMP22_MST_COIN_VALUE_UNDEFINED:
        return "UNDEFINED";
    case AMP22_MST_COIN_VALUE_HEAD:
        return "HEAD";
    case AMP22_MST_COIN_VALUE_TAIL:
        return "TAIL";
    default:
        return "";
    }
}

std::string AMP22MSTRoundPhaseActionType_to_string(
    AMP22MSTRoundPhaseActionType AMP22_MST_round_phase_action_type)
{
    switch (AMP22_MST_round_phase_action_type) {
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_MOE:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_MOE";
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_MOE:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_MOE";
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_MOE:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_MOE";
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE";

    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_"
               "COMMUNICATION_1_1";
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1_2:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_"
               "COMMUNICATION_1_1_2";
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_2:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_"
               "COMMUNICATION_1_2";

    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_TAIL:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_TAIL";
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL";
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_TAIL:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_TAIL";
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL";

    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_"
               "COMMUNICATION_2_1";
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1_2:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_"
               "COMMUNICATION_2_1_2";
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_2:
        return "AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_"
               "COMMUNICATION_2_2";
    }
    return "";
}
