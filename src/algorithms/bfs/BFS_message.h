#ifndef SCDS_ALGORITHMS_BFS_BFS_MESSAGE_H_
#define SCDS_ALGORITHMS_BFS_BFS_MESSAGE_H_

#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

enum BFSMessageType
{
    BFS_MESSAGE_SEARCH,
    BFS_MESSAGE_PARENT,
    BFS_MESSAGE_COMPLETE,
    BFS_MESSAGE_BROADCAST_LEVEL
};

class BFSMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return BFS_MESSAGE; }

    int sender_id = -1;
    BFSMessageType bfs_message_type;
    int level = -1;
    int parent_id = -1;

    void copyInformation(int sender_id, BFSMessageType bfs_message_type,
                         int level, int parent_id);

  public:
    BFSMessage(const char *name = nullptr, short kind = BFS_MESSAGE);
    BFSMessage(const BFSMessage &other);
    BFSMessage &operator=(const BFSMessage &other);
    virtual BFSMessage *dup() const override { return new BFSMessage(*this); }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    BFSMessageType getBFSMessageType() const;
    void setBFSMessageType(BFSMessageType bfs_message_type);
    int getLevel() const;
    void setLevel(int level);
    int getParentId() const;
    void setParentId(int parent_id);
    std::string to_string(int log_level = 0) const;
};

std::string BFSMessageType_to_string(BFSMessageType bfs_message_type);

#endif // SCDS_ALGORITHMS_BFS_BFS_MESSAGE_H_