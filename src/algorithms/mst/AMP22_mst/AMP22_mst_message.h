#ifndef SCDS_ALGORITHMS_MST_AMP22_MST_AMP22_MST_MESSAGE_H_
#define SCDS_ALGORITHMS_MST_AMP22_MST_AMP22_MST_MESSAGE_H_

#include "algorithms/ialg_node.h"
#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

enum AMP22MSTCoinValue
{
    AMP22_MST_COIN_VALUE_UNDEFINED,
    AMP22_MST_COIN_VALUE_HEAD,
    AMP22_MST_COIN_VALUE_TAIL
};

enum AMP22MSTRoundPhaseActionType
{
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_MOE,
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_MOE,
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_MOE,
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE,
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1, // send
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1_2,
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_2, // receive
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_TAIL,
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL,
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_TAIL,
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL,
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1, // send
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1_2,
    AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_2 // receive
};

class AMP22MSTMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return AMP22_MST_MESSAGE; }

    int sender_id = -1;
    int receiver_id = -2;
    int sent_round_id = -1;
    AMP22MSTRoundPhaseActionType round_action_type;

    int fragment_id = -1;
    int level = -1;
    int parent = -2;

    AMP22MSTCoinValue coin_value = AMP22_MST_COIN_VALUE_UNDEFINED;
    std::tuple<int, int, int> moe = INF_EDGE_ID;

    int new_fragment_id = -1;
    int new_level = -1;

    int real_sender_id = -1;
    int real_receiver_id = -2;
    bool is_MST_edge = false;

    void copyInformation(int sender_id, int receiver_id, int sent_round_id,
                         int fragment_id, int level, int parent,
                         AMP22MSTCoinValue coin_value,
                         const std::tuple<int, int, int> &moe,
                         int new_fragment_id, int new_level, int real_sender_id,
                         int real_receiver_id, bool is_MST_edge,
                         AMP22MSTRoundPhaseActionType round_action_type);

    std::string to_string(int log_level = 0) const;

  public:
    AMP22MSTMessage(const char *name = nullptr, short kind = AMP22_MST_MESSAGE);
    AMP22MSTMessage(const AMP22MSTMessage &other);
    AMP22MSTMessage &operator=(const AMP22MSTMessage &other);
    virtual AMP22MSTMessage *dup() const override
    {
        return new AMP22MSTMessage(*this);
    }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    int getReceiverId() const;
    void setReceiverId(int receiver_id);
    int getSentRoundId() const;
    void setSentRoundId(int sent_round_id);
    int getFragmentId() const;
    void setFragmentId(int fragment_id);
    int getLevel() const;
    void setLevel(int level);
    int getParent() const;
    void setParent(int parent);
    AMP22MSTCoinValue getCoinValue() const;
    void setCoinValue(AMP22MSTCoinValue coin_value);
    std::tuple<int, int, int> getMOE() const;
    void setMOE(const std::tuple<int, int, int> &moe);
    int getNewFragmentId() const;
    void setNewFragmentId(int new_fragment_id);
    int getNewLevel() const;
    void setNewLevel(int new_level);
    int getRealSenderId() const;
    void setRealSenderId(int real_sender_id);
    int getRealReceiverId() const;
    void setRealReceiverId(int real_receiver_id);
    bool getIsMSTEdge() const;
    void setIsMSTEdge(bool is_MST_edge);
    AMP22MSTRoundPhaseActionType getRoundActionType() const;
    void setRoundActionType(AMP22MSTRoundPhaseActionType round_action_type);
};

std::string AMP22MSTCoinValue_to_string(AMP22MSTCoinValue coin_value);
std::string AMP22MSTRoundPhaseActionType_to_string(
    AMP22MSTRoundPhaseActionType AMP22_MST_round_phase_action_type);

#endif // SCDS_ALGORITHMS_MST_AMP22_MST_AMP22_MST_MESSAGE_H_