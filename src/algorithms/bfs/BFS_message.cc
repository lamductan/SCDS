#include "algorithms/bfs/BFS_message.h"

BFSMessage::BFSMessage(const char *name, short kind) : cMessage(name, kind) {}

void BFSMessage::copyInformation(int sender_id, BFSMessageType bfs_message_type,
                                 int level, int parent_id)
{
    this->sender_id = sender_id;
    this->bfs_message_type = bfs_message_type;
    this->level = level;
    this->parent_id = parent_id;
}

BFSMessage::BFSMessage(const BFSMessage &other) : cMessage((cMessage)other)
{
    copyInformation(other.sender_id, other.bfs_message_type, other.level,
                    other.parent_id);
}

BFSMessage &BFSMessage::operator=(const BFSMessage &other)
{
    cMessage::operator=(other);
    copyInformation(other.sender_id, other.bfs_message_type, other.level,
                    other.parent_id);
    return *this;
}

int BFSMessage::getSenderId() const { return sender_id; }

void BFSMessage::setSenderId(int sender_id) { this->sender_id = sender_id; }

BFSMessageType BFSMessage::getBFSMessageType() const
{
    return bfs_message_type;
}

void BFSMessage::setBFSMessageType(BFSMessageType bfs_message_type)
{
    this->bfs_message_type = bfs_message_type;
}

int BFSMessage::getLevel() const { return level; }

void BFSMessage::setLevel(int level) { this->level = level; }

int BFSMessage::getParentId() const { return parent_id; }

void BFSMessage::setParentId(int parent_id) { this->parent_id = parent_id; }

std::string BFSMessage::to_string(int log_level) const
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "bfs_message = {\n";
    s += (tab + one_tab + "bfs_message_type = " +
          BFSMessageType_to_string(bfs_message_type) + ",\n");
    s += (tab + one_tab + "sender_id        = " + std::to_string(sender_id) +
          ",\n");
    s +=
        (tab + one_tab + "level            = " + std::to_string(level) + ",\n");
    s += (tab + one_tab + "parent_id        = " + std::to_string(parent_id) +
          ",\n");
    return (s += tab + "}");
}

std::string BFSMessageType_to_string(BFSMessageType bfs_message_type)
{
    switch (bfs_message_type) {
    case BFS_MESSAGE_SEARCH:
        return "BFS_MESSAGE_SEARCH";
    case BFS_MESSAGE_PARENT:
        return "BFS_MESSAGE_PARENT";
    case BFS_MESSAGE_COMPLETE:
        return "BFS_MESSAGE_COMPLETE";
    case BFS_MESSAGE_BROADCAST_LEVEL:
        return "BFS_MESSAGE_BROADCAST_LEVEL";
    default:
        return "";
    }
}