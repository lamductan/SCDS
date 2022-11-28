#ifndef SCDS_ALGORITHMS_MST_GHS_MST_MESSAGE_H_
#define SCDS_ALGORITHMS_MST_GHS_MST_MESSAGE_H_

#include "algorithms/ialg_node.h"
#include "centralized/graph/edge.h"
#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

enum GHSMSTMessageType
{
    GHS_MST_MESSAGE_NIL,
    GHS_MST_MESSAGE_CONNECT,
    GHS_MST_MESSAGE_INITIATE,
    GHS_MST_MESSAGE_TEST,
    GHS_MST_MESSAGE_ACCEPT,
    GHS_MST_MESSAGE_REJECT,
    GHS_MST_MESSAGE_REPORT,
    GHS_MST_MESSAGE_CHANGE_ROOT,
    GHS_MST_MESSAGE_DONE
};

enum GHSMSTNodeState
{
    GHS_MST_NODE_STATE_NIL,
    GHS_MST_NODE_STATE_FOUND,
    GHS_MST_NODE_STATE_FIND,
    GHS_MST_NODE_STATE_DONE
};

enum GHSMSTEdgeState
{
    GHS_MST_EDGE_STATE_NIL,
    GHS_MST_EDGE_STATE_BASIC,
    GHS_MST_EDGE_STATE_BRANCH,
    GHS_MST_EDGE_STATE_REJECTED
};

const std::tuple<int, int, int> INF_EDGE_ID = { INT_MAX, -1, -1 };

class GHSMSTMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return GHS_MST_MESSAGE; }

    int sender_id = -1;
    int receiver_id = -1;
    std::tuple<int, int, int> edge_id;
    int sent_round_id;
    std::pair<std::tuple<int, int, int>, int> fragment_core = { INF_EDGE_ID,
                                                                -1 };
    int level = -1;
    GHSMSTNodeState GHS_MST_node_state = GHS_MST_NODE_STATE_FIND;
    GHSMSTMessageType GHS_MST_message_type;
    std::tuple<int, int, int> best_edge_id = INF_EDGE_ID;
    int fragment_id;

    int real_sender_id = -1;
    int real_receiver_id = -1;
    int prev_node_on_path_id = -1;
    int next_node_on_path_id = -1;

    int message_id;

    void copyInformation(
        int sender_id, int receiver_id,
        const std::tuple<int, int, int> &edge_id, int sent_round_id,
        const std::pair<std::tuple<int, int, int>, int> &fragment_core,
        int level, GHSMSTNodeState GHS_MST_node_state,
        GHSMSTMessageType GHS_MST_message_type,
        const std::tuple<int, int, int> &best_edge_id, int fragment_id,
        int real_sender_id, int real_receiver_id, int message_id,
        int prev_node_on_path_id, int next_node_on_path_id);

    std::string to_string(int log_level = 0) const;

  public:
    GHSMSTMessage(const char *name = nullptr, short kind = GHS_MST_MESSAGE);
    GHSMSTMessage(const GHSMSTMessage &other);
    GHSMSTMessage &operator=(const GHSMSTMessage &other);
    virtual GHSMSTMessage *dup() const override
    {
        return new GHSMSTMessage(*this);
    }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    int getReceiverId() const;
    void setReceiverId(int receiver_id);
    std::tuple<int, int, int> getEdgeId() const;
    void setEdgeId(const std::tuple<int, int, int> &edge_id);
    int getSentRoundId() const;
    void setSentRoundId(int sent_round_id);
    std::pair<std::tuple<int, int, int>, int> getFragmentCore() const;
    void
    setFragmentCore(std::pair<std::tuple<int, int, int>, int> fragment_core);
    int getLevel() const;
    void setLevel(int level);
    GHSMSTNodeState getGHSMSTNodeState() const;
    void setGHSMSTNodeState(GHSMSTNodeState GHS_MST_node_state);
    GHSMSTMessageType getGHSMSTMessageType() const;
    void setGHSMSTMessageType(GHSMSTMessageType GHS_MST_message_type);
    std::tuple<int, int, int> getBestEdgeId() const;
    void setBestEdgeId(const std::tuple<int, int, int> &best_edge_id);
    int getFragmentId() const;
    void setFragmentId(int fragment_id);
    int getRealSenderId() const;
    void setRealSenderId(int real_sender_id);
    int getRealReceiverId() const;
    void setRealReceiverId(int real_receiver_id);
    int getMessageId() const;
    void setMessageId(int message_id);
    int getPrevNodeOnPathId() const;
    void setPrevNodeOnPathId(int prev_node_on_path_id);
    int getNextNodeOnPathId() const;
    void setNextNodeOnPathId(int next_node_on_path_id);
};

std::string GHSMSTMessageType_to_string(GHSMSTMessageType GHS_MST_message_type);
std::string GHSMSTNodeState_to_string(GHSMSTNodeState GHS_MST_node_state);
std::string GHSMSTEdgeState_to_string(GHSMSTEdgeState GHS_MST_edge_state);
#endif