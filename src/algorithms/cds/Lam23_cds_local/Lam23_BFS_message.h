#ifndef SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_MESSAGE_H_
#define SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_MESSAGE_H_

#include "algorithms/ialg_node.h"
#include "centralized/graph/graph.h"
#include "messages/message_types.h"
#include <omnetpp.h>

using namespace omnetpp;

enum LamBFSMessageType
{
    LAM_BFS_MESSAGE_DEFAULT,
    LAM_BFS_MESSAGE_UPCAST_MOE,
    LAM_BFS_MESSAGE_BROADCAST_MOE,
    LAM_BFS_MESSAGE_INTER_FRAGMENT_INFORM_BFS_1,
    LAM_BFS_MESSAGE_UPCAST_BFS,
    LAM_BFS_MESSAGE_BROADCAST_BFS,
    LAM_BFS_MESSAGE_INTER_FRAGMENT_INFORM_BFS_2,
    LAM_BFS_MESSAGE_UPCAST_FAILED,
    LAM_BFS_MESSAGE_BROADCAST_FAILED
};

enum LamBFSCoinValue
{
    LAM_BFS_COIN_VALUE_UNDEFINED,
    LAM_BFS_COIN_VALUE_HEAD,
    LAM_BFS_COIN_VALUE_TAIL
};

struct LamBFSFragmentInformation
{
    int fragment_id = -1;
    bool fragment_status = true;
    int fragment_diameter = -1;
    std::set<int> fragment_nodes;
    std::set<std::tuple<int, int, int>> edges_adj_fragment_nodes;
    std::map<int, centralized::BFSTreeStructure> fragment_bfs_tree_structure;

    std::string to_string(int log_level = 0,
                          bool log_level_on_first_line = true) const;
};

class LamBFSMessage : public cMessage
{
  public:
    static MessageType getMessageType() { return LAM_BFS_MESSAGE; }

    LamBFSMessageType LAM_BFS_message_type = LAM_BFS_MESSAGE_DEFAULT;
    int sender_id = -1;
    int sent_round_id = -1;
    LamBFSFragmentInformation fragment_information;
    LamBFSFragmentInformation new_fragment_information;
    LamBFSCoinValue coin_value = LAM_BFS_COIN_VALUE_UNDEFINED;
    std::tuple<int, int, int> moe;

    std::set<int> new_fragment_nodes;
    std::set<std::tuple<int, int, int>> new_edges_adj_fragment_nodes;

    int prev_node_on_path_id = -1;
    int next_node_on_path_id = -1;

    void copyInformation(int sender_id, int sent_round_id,
                         LamBFSMessageType LAM_BFS_message_type,
                         LamBFSFragmentInformation fragment_information,
                         LamBFSFragmentInformation new_fragment_information,
                         LamBFSCoinValue coin_value,
                         const std::tuple<int, int, int> &moe,
                         int prev_node_on_path_id, int next_node_on_path_id);

    std::string to_string(int log_level = 0) const;

  public:
    LamBFSMessage(const char *name = nullptr, short kind = LAM_BFS_MESSAGE);
    LamBFSMessage(const LamBFSMessage &other);
    LamBFSMessage &operator=(const LamBFSMessage &other);
    virtual LamBFSMessage *dup() const override
    {
        return new LamBFSMessage(*this);
    }

    // field getter/setter methods
    int getSenderId() const;
    void setSenderId(int sender_id);
    int getSentRoundId() const;
    void setSentRoundId(int sent_round_id);
    LamBFSMessageType getLamBFSMessageType() const;
    void setLamBFSMessageType(LamBFSMessageType LAM_BFS_message_type);

    LamBFSFragmentInformation getFragmentInformation() const;
    void setFragmentInformation(
        const LamBFSFragmentInformation &fragment_information);

    LamBFSFragmentInformation getNewFragmentInformation() const;
    void setNewFragmentInformation(
        const LamBFSFragmentInformation &new_fragment_information);

    LamBFSCoinValue getCoinValue() const;
    void setCoinValue(LamBFSCoinValue coin_value);
    std::tuple<int, int, int> getMOE() const;
    void setMOE(const std::tuple<int, int, int> &moe);
    int getPrevNodeOnPathId() const;
    void setPrevNodeOnPathId(int prev_node_on_path_id);
    int getNextNodeOnPathId() const;
    void setNextNodeOnPathId(int next_node_on_path_id);
};

std::string LamBFSMessageType_to_string(LamBFSMessageType LAM_BFS_message_type);
std::string LamBFSCoinValue_to_string(LamBFSCoinValue coin_value);

#endif // SCDS_ALGORITHMS_CDS_LAM_CDS_LOCAL_LAM_BFS_MESSAGE_H_