#ifndef SCDS_ALGORITHMS_BFS_BFS_H_
#define SCDS_ALGORITHMS_BFS_BFS_H_

#include "algorithms/bfs/BFS_message.h"
#include "algorithms/ialg_node.h"

class BFSAlg : public IAlgNode
{
  public:
    bool marked = false;
    int parent_id = INT_MAX;
    int level = INT_MAX;

    std::set<int> children;
    std::set<int> all_remained_neighbors;
    std::set<int> completed_children;

    bool sent_search_message = false;
    bool is_completed = false;

    std::set<int> responded_search_neighbors;
    std::set<int> received_search_from;
    std::set<int> received_search_from_this_round;
    bool is_decided_current_round = false;
    bool is_decided_previous_round = false;

    std::set<std::pair<int, int>> neighbors_rank;

    void process_search_message(BFSMessage *bfs_message);
    void process_parent_message(BFSMessage *bfs_message);
    void process_complete_message(BFSMessage *bfs_message);
    void process_broadcast_level_message(BFSMessage *bfs_message);

  public:
    virtual void set_alg_type() override;
    BFSAlg(Node *node, int starting_round = 1);
    virtual void process_round() override;
    void process_bfs_message_queue();
    std::vector<std::pair<BFSMessage *, std::set<int>>> create_new_messages();
    void send_new_messages(
        const std::vector<std::pair<BFSMessage *, std::set<int>>> &messages);
    virtual bool is_selected() override;
    virtual bool is_awake() override;
    virtual bool is_decided() override;

    BFSMessage *create_bfs_message();
    virtual centralized::BFSTreeStructure
    get_bfs_tree_structure() const override;

    void print_state(int log_level = 0);
};

#endif // SCDS_ALGORITHMS_BFS_BFS_H_