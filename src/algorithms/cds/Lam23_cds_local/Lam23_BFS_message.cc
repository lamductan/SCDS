#include "algorithms/cds/Lam23_cds_local/Lam23_BFS_message.h"
#include "utils/utils.h"

LamBFSMessage::LamBFSMessage(const char *name, short kind)
    : cMessage(name, kind)
{
}

void LamBFSMessage::copyInformation(
    int sender_id, int sent_round_id, LamBFSMessageType LAM_BFS_message_type,
    LamBFSFragmentInformation fragment_information,
    LamBFSFragmentInformation new_fragment_information,
    LamBFSCoinValue coin_value, const std::tuple<int, int, int> &moe,
    int prev_node_on_path_id, int next_node_on_path_id)
{
    this->sender_id = sender_id;
    this->sent_round_id = sent_round_id;
    this->LAM_BFS_message_type = LAM_BFS_message_type;
    this->fragment_information = fragment_information;
    this->new_fragment_information = new_fragment_information;
    this->coin_value = coin_value;
    this->moe = moe;
    this->prev_node_on_path_id = prev_node_on_path_id;
    this->next_node_on_path_id = next_node_on_path_id;
}

LamBFSMessage::LamBFSMessage(const LamBFSMessage &other)
    : cMessage((cMessage)other)
{
    copyInformation(other.sender_id, other.sent_round_id,
                    other.LAM_BFS_message_type, other.fragment_information,
                    other.new_fragment_information, other.coin_value, other.moe,
                    other.prev_node_on_path_id, other.next_node_on_path_id);
}

LamBFSMessage &LamBFSMessage::operator=(const LamBFSMessage &other)
{
    cMessage::operator=(other);
    copyInformation(other.sender_id, other.sent_round_id,
                    other.LAM_BFS_message_type, other.fragment_information,
                    other.new_fragment_information, other.coin_value, other.moe,
                    other.prev_node_on_path_id, other.next_node_on_path_id);
    return *this;
}

int LamBFSMessage::getSenderId() const { return sender_id; }

void LamBFSMessage::setSenderId(int sender_id) { this->sender_id = sender_id; }

int LamBFSMessage::getSentRoundId() const { return sent_round_id; }

void LamBFSMessage::setSentRoundId(int sent_round_id)
{
    this->sent_round_id = sent_round_id;
}

LamBFSMessageType LamBFSMessage::getLamBFSMessageType() const
{
    return LAM_BFS_message_type;
}

void LamBFSMessage::setLamBFSMessageType(LamBFSMessageType LAM_BFS_message_type)
{
    this->LAM_BFS_message_type = LAM_BFS_message_type;
}

LamBFSFragmentInformation LamBFSMessage::getFragmentInformation() const
{
    return fragment_information;
}

void LamBFSMessage::setFragmentInformation(
    const LamBFSFragmentInformation &fragment_information)
{
    this->fragment_information = fragment_information;
}

LamBFSFragmentInformation LamBFSMessage::getNewFragmentInformation() const
{
    return new_fragment_information;
}

void LamBFSMessage::setNewFragmentInformation(
    const LamBFSFragmentInformation &new_fragment_information)
{
    this->new_fragment_information = new_fragment_information;
}

LamBFSCoinValue LamBFSMessage::getCoinValue() const { return coin_value; }

void LamBFSMessage::setCoinValue(LamBFSCoinValue coin_value)
{
    this->coin_value = coin_value;
}

std::tuple<int, int, int> LamBFSMessage::getMOE() const { return moe; }

void LamBFSMessage::setMOE(const std::tuple<int, int, int> &moe)
{
    this->moe = moe;
}

int LamBFSMessage::getPrevNodeOnPathId() const { return prev_node_on_path_id; }

void LamBFSMessage::setPrevNodeOnPathId(int prev_node_on_path_id)
{
    this->prev_node_on_path_id = prev_node_on_path_id;
}

int LamBFSMessage::getNextNodeOnPathId() const { return next_node_on_path_id; }

void LamBFSMessage::setNextNodeOnPathId(int next_node_on_path_id)
{
    this->next_node_on_path_id = next_node_on_path_id;
}

std::string LamBFSMessage::to_string(int log_level) const
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "LAM_BFS_message = {\n";
    s += (tab + one_tab + "LAM_BFS_message_type     = " +
          LamBFSMessageType_to_string(LAM_BFS_message_type) + ",\n");
    s += (tab + one_tab +
          "sender_id                = " + std::to_string(sender_id) + ",\n");
    s += (tab + one_tab + "sent_round_id            = " +
          std::to_string(sent_round_id) + ",\n");
    s += (tab + one_tab + "fragment_information     = " +
          fragment_information.to_string(log_level + 1, false) + ",\n");
    s += (tab + one_tab + "new_fragment_information = " +
          new_fragment_information.to_string(log_level + 1, false) + ",\n");
    s += (tab + one_tab + "coin_value               = " +
          LamBFSCoinValue_to_string(coin_value) + ",\n");
    s += (tab + one_tab + "moe                      = " + tuple_to_string(moe) +
          "\n");
    return (s += tab + "}");
}

std::string LamBFSMessageType_to_string(LamBFSMessageType LAM_BFS_message_type)
{
    switch (LAM_BFS_message_type) {
    case LAM_BFS_MESSAGE_DEFAULT:
        return "LAM_BFS_MESSAGE_DEFAULT";
    case LAM_BFS_MESSAGE_UPCAST_MOE:
        return "LAM_BFS_MESSAGE_UPCAST_MOE";
    case LAM_BFS_MESSAGE_BROADCAST_MOE:
        return "LAM_BFS_MESSAGE_BROADCAST_MOE";
    case LAM_BFS_MESSAGE_INTER_FRAGMENT_INFORM_BFS_1:
        return "LAM_BFS_MESSAGE_INTER_FRAGMENT_INFORM_BFS_1";
    case LAM_BFS_MESSAGE_UPCAST_BFS:
        return "LAM_BFS_MESSAGE_UPCAST_BFS";
    case LAM_BFS_MESSAGE_BROADCAST_BFS:
        return "LAM_BFS_MESSAGE_BROADCAST_BFS";
    case LAM_BFS_MESSAGE_INTER_FRAGMENT_INFORM_BFS_2:
        return "LAM_BFS_MESSAGE_INTER_FRAGMENT_INFORM_BFS_2";
    case LAM_BFS_MESSAGE_UPCAST_FAILED:
        return "LAM_BFS_MESSAGE_UPCAST_FAILED";
    case LAM_BFS_MESSAGE_BROADCAST_FAILED:
        return "LAM_BFS_MESSAGE_BROADCAST_FAILED";
    default:
        return "";
    }
}

std::string LamBFSCoinValue_to_string(LamBFSCoinValue coin_value)
{
    switch (coin_value) {
    case LAM_BFS_COIN_VALUE_UNDEFINED:
        return "UNDEFINED";
    case LAM_BFS_COIN_VALUE_HEAD:
        return "HEAD";
    case LAM_BFS_COIN_VALUE_TAIL:
        return "TAIL";
    default:
        return "";
    }
}

std::string
LamBFSFragmentInformation::to_string(int log_level,
                                     bool log_level_on_first_line) const
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');

    std::string s = (log_level_on_first_line) ? tab + "{\n" : "{\n";

    s += (tab + one_tab + "fragment_id                 = " +
          std::to_string(fragment_id) + ",\n");
    s += (tab + one_tab + "fragment_status             = " +
          std::to_string(fragment_status) + ",\n");
    s += (tab + one_tab + "fragment_diameter           = " +
          std::to_string(fragment_diameter) + ",\n");
    s += (tab + one_tab + "fragment_nodes              = " +
          set_to_string<int>(fragment_nodes) + ",\n");
    s += (tab + one_tab + "edges_adj_fragment_nodes    = " +
          set_tuple_to_string(edges_adj_fragment_nodes, log_level + 1, false) +
          ",\n");
    s += (tab + one_tab + "fragment_bfs_tree_structure = " +
          map_to_string_custom<centralized::BFSTreeStructure>(
              fragment_bfs_tree_structure, log_level + 1, false) +
          ",\n");
    s += tab + "}";
    return s;
}