#include "algorithms/bfs/BFS.h"
#include "utils/utils.h"

void BFSAlg::set_alg_type()
{
    EV << "BFSAlg::set_alg_type()\n";
    alg_type = BFS_ALG;
}

BFSAlg::BFSAlg(Node *node, int starting_round)
{
    init(node, starting_round);
    all_remained_neighbors =
        std::set<int>(node->all_neighbors.begin(), node->all_neighbors.end());
    if (id == 0) {
        level = 0;
        parent_id = -1;
    }
}

bool BFSAlg::is_awake()
{
    awake_round_map[current_round_id] = true;
    return true;
}

bool BFSAlg::is_selected() { return true; }

bool BFSAlg::is_decided()
{
    is_decided_current_round = (all_remained_neighbors.empty() &&
                                completed_children.size() == children.size());
    return is_decided_current_round;
}

void BFSAlg::process_round()
{
    EV << "BFSAlg::process_round()\n";
    EV << "n_awake_rounds = " << n_awake_rounds << '\n';
    if (!is_awake()) {
        EV << "not awake\n";
        return;
    }
    ++n_awake_rounds;
    is_decided_previous_round = is_decided_current_round;
    EV << "Is awake: n_awake_rounds = " << n_awake_rounds << '\n';
    EV << "pre_process: message_queue.size() = " << message_queue.size()
       << '\n';
    process_bfs_message_queue();
    std::vector<std::pair<BFSMessage *, std::set<int>>> sent_messages =
        create_new_messages();
    // print_state();
    send_new_messages(sent_messages);
    clear_message_queue();
}

void BFSAlg::process_bfs_message_queue()
{
    EV << "\t"
       << "BFSAlg::process_bfs_message_queue()\n";
    received_search_from_this_round.clear();
    for (cMessage *msg : message_queue) {
        BFSMessage *bfs_message = dynamic_cast<BFSMessage *>(msg);
        // EV << bfs_message->to_string(1) << '\n';
        switch (bfs_message->getBFSMessageType()) {
        case BFS_MESSAGE_SEARCH:
            process_search_message(bfs_message);
            break;
        case BFS_MESSAGE_PARENT:
            process_parent_message(bfs_message);
            break;
        case BFS_MESSAGE_COMPLETE:
            process_complete_message(bfs_message);
            break;
        case BFS_MESSAGE_BROADCAST_LEVEL:
            process_broadcast_level_message(bfs_message);
            break;
        default:
            break;
        }
    }
}

void BFSAlg::process_search_message(BFSMessage *bfs_message)
{
    int sender_id = bfs_message->getSenderId();
    int received_level = bfs_message->getLevel();
    received_search_from_this_round.insert(sender_id);
    if (!marked) {
        marked = true;
    }

    int possible_new_level = received_level + 1;
    if (possible_new_level < level ||
        (possible_new_level == level && sender_id < parent_id)) {
        level = possible_new_level;
        parent_id = sender_id;
    }
    all_remained_neighbors.erase(sender_id);
}

void BFSAlg::process_parent_message(BFSMessage *bfs_message)
{
    int sender_id = bfs_message->getSenderId();
    responded_search_neighbors.insert(sender_id);
    all_remained_neighbors.erase(sender_id);
    int parent_id = bfs_message->getParentId();
    if (parent_id != id) return;
    children.insert(sender_id);
}

void BFSAlg::process_complete_message(BFSMessage *bfs_message)
{
    int sender_id = bfs_message->getSenderId();
    if (children.count(sender_id) == 0) return;
    completed_children.insert(sender_id);
}

void BFSAlg::process_broadcast_level_message(BFSMessage *bfs_message)
{
    int sender_id = bfs_message->getSenderId();
    int sender_level = bfs_message->getLevel();
    neighbors_rank.insert({ sender_level, sender_id });
}

BFSMessage *BFSAlg::create_bfs_message()
{
    BFSMessage *msg = new BFSMessage();
    msg->setSenderId(id);
    msg->setParentId(parent_id);
    msg->setLevel(level);
    return msg;
}

std::vector<std::pair<BFSMessage *, std::set<int>>>
BFSAlg::create_new_messages()
{
    std::vector<std::pair<BFSMessage *, std::set<int>>> new_messages;

    if (!received_search_from_this_round.empty()) {
        received_search_from = combine_set<int>(
            received_search_from, received_search_from_this_round);
        BFSMessage *respond_search_message = create_bfs_message();
        respond_search_message->setBFSMessageType(BFS_MESSAGE_PARENT);
        new_messages.push_back(
            { respond_search_message, received_search_from_this_round });
        // EV << "\tCreate respond_search_message (BFS_MESSAGE_PARENT): "
        //    << respond_search_message->to_string(1) << '\n';
    }

    if (level == INT_MAX) {
        return new_messages;
    }

    if (!all_remained_neighbors.empty()) {
        if (!sent_search_message) {
            BFSMessage *new_search_message = create_bfs_message();
            new_search_message->setBFSMessageType(BFS_MESSAGE_SEARCH);
            new_messages.push_back(
                { new_search_message, all_remained_neighbors });
            sent_search_message = true;
            // EV << "\tCreate new_search_message: "
            //    << new_search_message->to_string(1) << '\n';

            BFSMessage *new_broadcast_level_message = create_bfs_message();
            new_broadcast_level_message->setBFSMessageType(
                BFS_MESSAGE_BROADCAST_LEVEL);
            new_messages.push_back(
                { new_broadcast_level_message,
                  std::set<int>(node->all_neighbors.begin(),
                                node->all_neighbors.end()) });
            // EV << "\tCreate new_broadcast_level_message: "
            //    << new_broadcast_level_message->to_string(1) << '\n';
        }
    } else {
        if (completed_children.size() == children.size() && !is_completed) {
            if (level != 0) {
                BFSMessage *new_complete_message = create_bfs_message();
                new_complete_message->setBFSMessageType(BFS_MESSAGE_COMPLETE);
                new_messages.push_back({ new_complete_message, { parent_id } });
            }
            is_completed = true;
        }
    }

    return new_messages;
}

void BFSAlg::send_new_messages(
    const std::vector<std::pair<BFSMessage *, std::set<int>>> &messages)
{
    for (const std::pair<BFSMessage *, std::set<int>> &message : messages) {
        BFSMessage *bfs_message = message.first;
        std::set<int> need_to_send_bfs_message = message.second;
        for (int neighbor_id : need_to_send_bfs_message) {
            node->sendDelayed(bfs_message->dup(), DELAY_SENT,
                              node->neighbor_gates[neighbor_id]);
        }
        // EV << bfs_message->to_string() << '\n';
        // EV << "need_to_send_bfs_message = "
        //    << set_to_string<int>(need_to_send_bfs_message) << '\n';
        // EV << '\n';
        delete bfs_message;
    }
}

centralized::BFSTreeStructure BFSAlg::get_bfs_tree_structure() const
{
    return centralized::BFSTreeStructure(id, level, parent_id, children);
}

void BFSAlg::print_state(int log_level)
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "state = {\n";
    s += (tab + one_tab +
          "id                              = " + std::to_string(id) + ",\n");
    s += (tab + one_tab + "marked                          = " +
          std::to_string(marked) + ",\n");
    s += (tab + one_tab + "parent_id                       = " +
          std::to_string(parent_id) + ",\n");
    s += (tab + one_tab +
          "level                           = " + std::to_string(level) + ",\n");
    s += (tab + one_tab + "sent_search_message             = " +
          std::to_string(sent_search_message) + ",\n");
    s += (tab + one_tab + "children                        = " +
          set_to_string<int>(children) + ",\n");
    s += (tab + one_tab + "all_remained_neighbors          = " +
          set_to_string<int>(all_remained_neighbors) + ",\n");
    s += (tab + one_tab + "completed_children              = " +
          set_to_string<int>(completed_children) + ",\n");
    s += (tab + one_tab + "responded_search_neighbors      = " +
          set_to_string<int>(responded_search_neighbors) + ",\n");
    s += (tab + one_tab + "received_search_from            = " +
          set_to_string<int>(received_search_from) + ",\n");
    s += (tab + one_tab + "received_search_from_this_round = " +
          set_to_string<int>(received_search_from_this_round) + ",\n");
    s += (tab + one_tab + "is_decided()                    = " +
          std::to_string(is_decided()) + ",\n");
    s += (tab + one_tab + "is_completed                    = " +
          std::to_string(is_completed) + ",\n");
    s += (tab + one_tab + "neighbors_rank                  = " +
          set_pair_to_string<int, int>(neighbors_rank) + ",\n");
    s += (tab + "}");
    EV << s << '\n';
}