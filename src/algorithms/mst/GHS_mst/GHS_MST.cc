#include "algorithms/mst/GHS_mst/GHS_MST.h"
#include "utils/utils.h"

void GHSMSTAlg::set_alg_type()
{
    EV << "GHSMSTAlg::set_alg_type()\n";
    alg_type = MST_ALG;
}

GHSMSTAlg::GHSMSTAlg() {}

GHSMSTAlg::GHSMSTAlg(Node *node, int starting_round)
{
    init(node, starting_round);
}

void GHSMSTAlg::init(Node *node, int starting_round)
{
    IAlgNode::init(node, starting_round);
    for (int neighbor_id : node->all_neighbors) {
        centralized::Edge edge(id, neighbor_id);
        std::tuple<int, int, int> edge_id = edge.get_id();
        basic_edges_set.insert(edge_id);
        edges_map[edge_id] = edge;
        edges_state_map[edge_id] = GHS_MST_EDGE_STATE_BASIC;
    }

    fragment_core = { INF_EDGE_ID, id };
    minimum_outgoing_edge_id = *basic_edges_set.begin();
    add_branch_edge(minimum_outgoing_edge_id);
}

int GHSMSTAlg::get_neighbor_id(const std::tuple<int, int, int> &edge_id) const
{
    return get_neighbor_id_of_edge(edge_id, id);
}

bool GHSMSTAlg::is_endpoint_of_edge(
    const std::tuple<int, int, int> &edge_id) const
{
    return std::get<1>(edge_id) == id || std::get<2>(edge_id) == id;
}

int GHSMSTAlg::find_next_relay_node_id_on_path_to(int next_node_on_path_id)
{
    return next_node_on_path_id;
}

void GHSMSTAlg::add_branch_edge(const std::tuple<int, int, int> &edge_id)
{
    // EV << "\t"
    //    << "GHSMSTAlg::add_branch_edge()\n";
    // EV << "\t\t" << tuple_to_string(edge_id) << '\n';
    basic_edges_set.erase(edge_id);
    edges_state_map[edge_id] = GHS_MST_EDGE_STATE_BRANCH;
    branch_edges_set.insert(edge_id);
    int neighbor_id = get_neighbor_id(edge_id);
    tree_edges[neighbor_id] = edges_map[edge_id];
}

void GHSMSTAlg::add_rejected_edge(const std::tuple<int, int, int> &edge_id)
{
    // EV << "\t"
    //    << "GHSMSTAlg::add_rejected_edge()\n";
    // EV << "\t\t" << tuple_to_string(edge_id) << '\n';
    edges_state_map[edge_id] = GHS_MST_EDGE_STATE_REJECTED;
    basic_edges_set.erase(edge_id);
}

bool GHSMSTAlg::is_decided()
{
    bool res = basic_edges_set.empty();
    return res;
}

bool GHSMSTAlg::is_awake() { return true; }

GHSMSTMessage *
GHSMSTAlg::create_message(GHSMSTMessageType GHS_MST_message_type,
                          const std::tuple<int, int, int> &edge_id,
                          int real_sender_id, int real_receiver_id,
                          int prev_node_on_path_id, int next_node_on_path_id)
{
    int sender_id = id;
    if (prev_node_on_path_id == -1) prev_node_on_path_id = id;
    if (next_node_on_path_id == -1)
        next_node_on_path_id = get_neighbor_id(edge_id);
    int receiver_id = find_next_relay_node_id_on_path_to(next_node_on_path_id);
    if (real_receiver_id == -2) real_receiver_id = next_node_on_path_id;

    int sent_round_id = current_round_id;
    GHSMSTMessage *msg = new GHSMSTMessage();

    msg->setSenderId(sender_id);
    msg->setReceiverId(receiver_id);
    if (real_sender_id == -1) real_sender_id = sender_id;
    msg->setRealSenderId(real_sender_id);
    msg->setRealReceiverId(real_receiver_id);
    msg->setEdgeId(edge_id);
    msg->setGHSMSTMessageType(GHS_MST_message_type);
    msg->setFragmentId(fragment_core.second);
    msg->setSentRoundId(current_round_id);
    msg->setMessageId(message_id++);
    msg->setPrevNodeOnPathId(prev_node_on_path_id);
    msg->setNextNodeOnPathId(next_node_on_path_id);
    return msg;
}

void GHSMSTAlg::listen_new_message(cMessage *msg)
{
    async_message_queue.insert(dynamic_cast<GHSMSTMessage *>(msg));
    last_communication_round = current_round_id;
}

void GHSMSTAlg::send_messages_current_round()
{
    // EV << "GHSMSTAlg::send_messages_current_round()\n";
    // EV << "\t"
    //    << "need to send " << need_to_send_message_queue.size() <<
    //    "messages\n";
    std::set<int> already_sent_neighbors;
    std::vector<GHSMSTMessage *> sent_messages;
    if (!need_to_send_message_queue.empty())
        last_communication_round = current_round_id;
    for (GHSMSTMessage *msg : need_to_send_message_queue) {
        int receiver_id = msg->getReceiverId();
        if (already_sent_neighbors.count(receiver_id)) continue;
        msg->setSentRoundId(current_round_id);
        // EV << msg->to_string(1) << '\n';
        node->sendDelayed(msg->dup(), DELAY_SENT,
                          node->neighbor_gates[receiver_id]);
        sent_messages.push_back(msg);
    }
    for (GHSMSTMessage *msg : sent_messages) {
        need_to_send_message_queue.erase(msg);
        delete msg;
    }
}

void GHSMSTAlg::clear_message_queue()
{
    for (GHSMSTMessage *msg : need_to_send_message_queue) {
        delete msg;
    }
    need_to_send_message_queue.clear();
    for (GHSMSTMessage *msg : async_message_queue) {
        delete msg;
    }
    async_message_queue.clear();
}

void GHSMSTAlg::process_round()
{
    if (!is_awake()) return;
    // EV << "GHSMSTAlg::process_round()\n";
    // print_state();
    awake_round_map[current_round_id] = true;
    ++n_awake_rounds;
    process_async_message_queue();
    send_messages_current_round();
}

void GHSMSTAlg::process_async_message_queue()
{
    EV << "GHSMSTAlg::process_async_message_queue()\n";

    if (current_round_id == starting_round) {
        GHSMSTMessage *msg =
            create_message(GHS_MST_MESSAGE_CONNECT, minimum_outgoing_edge_id);
        msg->setLevel(level);
        need_to_send_message_queue.insert(msg);
        return;
    }

    std::set<int> already_processed_neighbors;
    std::vector<GHSMSTMessage *> processed_messages;
    for (GHSMSTMessage *msg : async_message_queue) {
        int sender_id = msg->getRealSenderId();
        if (already_processed_neighbors.count(sender_id) > 0) continue;
        // EV << msg->to_string(1) << '\n';
        bool processed = false;
        switch (msg->getGHSMSTMessageType()) {
        case GHS_MST_MESSAGE_CONNECT:
            processed = handle_connect_message(msg);
            break;
        case GHS_MST_MESSAGE_INITIATE:
            processed = handle_initiate_message(msg);
            break;
        case GHS_MST_MESSAGE_TEST:
            processed = handle_test_message(msg);
            break;
        case GHS_MST_MESSAGE_ACCEPT:
            processed = handle_accept_message(msg);
            break;
        case GHS_MST_MESSAGE_REJECT:
            processed = handle_reject_message(msg);
            break;
        case GHS_MST_MESSAGE_REPORT:
            processed = handle_report_message(msg);
            break;
        case GHS_MST_MESSAGE_CHANGE_ROOT:
            processed = handle_change_root_message(msg);
            break;
        case GHS_MST_MESSAGE_DONE:
            processed = handle_done_message(msg);
            break;
        default:
            break;
        }

        if (processed) {
            already_processed_neighbors.insert(sender_id);
            processed_messages.push_back(msg);
        }

        // EV << "\n";
    }

    for (GHSMSTMessage *msg : processed_messages) {
        async_message_queue.erase(msg);
        delete msg;
    }
}

bool GHSMSTAlg::handle_connect_message(GHSMSTMessage *msg)
{
    // EV << "\t"
    //    << "GHSMSTAlg::handle_connect_message()\n";
    int l = msg->getLevel();
    std::tuple<int, int, int> edge_id = msg->getEdgeId();
    GHSMSTMessage *new_msg = nullptr;
    if (l < level) {
        add_branch_edge(edge_id);
        new_msg = create_message(GHS_MST_MESSAGE_INITIATE, edge_id);
        new_msg->setLevel(level);
        new_msg->setFragmentCore(fragment_core);
        new_msg->setGHSMSTNodeState(GHS_MST_node_state);
        if (GHS_MST_node_state == GHS_MST_NODE_STATE_FIND) ++find_count;
        // EV << "\t\t"
        //    << "Absorb\n";
    } else if (edges_state_map[edge_id] == GHS_MST_EDGE_STATE_BASIC) {
        // EV << "\t\t"
        //    << "Delay\n";
        return false;
    } else {
        int new_fragment_id =
            std::max(fragment_core.second, msg->getFragmentId());
        fragment_core.second = new_fragment_id;
        new_msg = create_message(GHS_MST_MESSAGE_INITIATE, edge_id);
        new_msg->setLevel(level + 1);
        new_msg->setFragmentCore({ edge_id, new_fragment_id });
        new_msg->setGHSMSTNodeState(GHS_MST_NODE_STATE_FIND);
        // EV << "\t\t"
        //    << "Merge\n";
    }
    need_to_send_message_queue.insert(new_msg);
    // EV << "\t\t"
    //    << "need_to_send_message_queue.size() = "
    //    << need_to_send_message_queue.size() << '\n';
    return true;
}

bool GHSMSTAlg::handle_initiate_message(GHSMSTMessage *msg)
{
    // EV << "\t"
    //    << "GHSMSTAlg::handle_initiate_message()\n";
    std::tuple<int, int, int> edge_id = msg->getEdgeId();
    level = msg->getLevel();
    fragment_core = msg->getFragmentCore();
    GHS_MST_node_state = msg->getGHSMSTNodeState();
    in_branch = edge_id;

    best_edge_id = INF_EDGE_ID;
    for (const std::tuple<int, int, int> edge_id_1 : branch_edges_set) {
        if (edge_id_1 == edge_id) continue;
        GHSMSTMessage *new_msg =
            create_message(GHS_MST_MESSAGE_INITIATE, edge_id_1);
        new_msg->setLevel(level);
        new_msg->setFragmentCore(fragment_core);
        new_msg->setGHSMSTNodeState(GHS_MST_node_state);
        need_to_send_message_queue.insert(new_msg);
        if (GHS_MST_node_state == GHS_MST_NODE_STATE_FIND) ++find_count;
    }
    if (GHS_MST_node_state == GHS_MST_NODE_STATE_FIND) test();
    return true;
}

void GHSMSTAlg::test()
{
    // EV << "\t"
    //    << "GHSMSTAlg::test()\n";
    if (basic_edges_set.empty()) {
        test_edge_id = INF_EDGE_ID;
        report();
    } else {
        test_edge_id = *basic_edges_set.begin();
        // EV << "\t\t"
        //    << "test_edge_id = " << tuple_to_string(test_edge_id) << '\n';
        GHSMSTMessage *new_msg =
            create_message(GHS_MST_MESSAGE_TEST, test_edge_id);
        new_msg->setLevel(level);
        new_msg->setFragmentCore(fragment_core);
        need_to_send_message_queue.insert(new_msg);
    }
}

bool GHSMSTAlg::handle_test_message(GHSMSTMessage *msg)
{
    // EV << "\t"
    //    << "GHSMSTAlg::handle_test_message()\n";
    int l = msg->getLevel();
    if (l > level) {
        // EV << "\t\t"
        //    << "Delay\n";
        return false;
    }

    std::tuple<int, int, int> edge_id = msg->getEdgeId();

    // EV << "\t\t"
    //    << "fragment_core = {" << tuple_to_string(fragment_core.first) << ","
    //    << std::to_string(fragment_core.second) << "}\n";

    if (fragment_core != msg->getFragmentCore()) {
        // EV << "\t\t"
        //    << "ACCEPT edge" << tuple_to_string(edge_id) << '\n';
        GHSMSTMessage *new_msg =
            create_message(GHS_MST_MESSAGE_ACCEPT, edge_id);
        need_to_send_message_queue.insert(new_msg);
        return true;
    }

    if (edges_state_map[edge_id] == GHS_MST_EDGE_STATE_BASIC) {
        // EV << "\t\t"
        //    << "REJECT edge" << tuple_to_string(edge_id) << '\n';
        add_rejected_edge(edge_id);
    }

    // EV << "\t\t"
    //    << "test_edge_id = " << tuple_to_string(test_edge_id) << '\n';

    if (test_edge_id != edge_id) {
        // EV << "\t\t"
        //    << "Send REJECT on edge" << tuple_to_string(edge_id) << '\n';
        GHSMSTMessage *new_msg =
            create_message(GHS_MST_MESSAGE_REJECT, edge_id);
        need_to_send_message_queue.insert(new_msg);
    } else {
        // EV << "\t\t"
        //    << "Execute TEST\n";
        test();
    }
    return true;
}

bool GHSMSTAlg::handle_accept_message(GHSMSTMessage *msg)
{
    // EV << "\t"
    //    << "GHSMSTAlg::handle_accept_message()\n";
    test_edge_id = INF_EDGE_ID;
    std::tuple<int, int, int> edge_id = msg->getEdgeId();
    if (edge_id < best_edge_id) best_edge_id = edge_id;
    report();
    return true;
}

bool GHSMSTAlg::handle_reject_message(GHSMSTMessage *msg)
{
    // EV << "\t"
    //    << "GHSMSTAlg::handle_reject_message()\n";
    std::tuple<int, int, int> edge_id = msg->getEdgeId();
    if (edges_state_map[edge_id] == GHS_MST_EDGE_STATE_BASIC) {
        add_rejected_edge(edge_id);
    }
    test();
    return true;
}

void GHSMSTAlg::report()
{
    // EV << "\t"
    //    << "GHSMSTAlg::report()\n";
    if (find_count == 0 && test_edge_id == INF_EDGE_ID) {
        GHS_MST_node_state = GHS_MST_NODE_STATE_FOUND;
        GHSMSTMessage *new_msg =
            create_message(GHS_MST_MESSAGE_REPORT, in_branch);
        new_msg->setBestEdgeId(best_edge_id);
        new_msg->setFragmentCore(fragment_core);
        need_to_send_message_queue.insert(new_msg);
        // EV << "\t\t"
        //    << "sent one more message\n";
    }
}

bool GHSMSTAlg::handle_report_message(GHSMSTMessage *msg)
{
    // EV << "\t"
    //    << "GHSMSTAlg::handle_report_message()\n";
    std::tuple<int, int, int> edge_id = msg->getEdgeId();
    std::tuple<int, int, int> reported_best_edge_id = msg->getBestEdgeId();
    if (edge_id != in_branch) {
        --find_count;
        if (reported_best_edge_id < best_edge_id) {
            best_edge_id = reported_best_edge_id;
            received_best_edge_from = msg->getRealSenderId();
        }
        report();
        return true;
    } else {
        if (GHS_MST_node_state == GHS_MST_NODE_STATE_FIND) {
            return false;
        }

        if (reported_best_edge_id == INF_EDGE_ID &&
            best_edge_id == INF_EDGE_ID) {
            GHS_MST_node_state = GHS_MST_NODE_STATE_DONE;
        } else if (reported_best_edge_id > best_edge_id ||
                   (reported_best_edge_id == best_edge_id &&
                    fragment_core.second > msg->getFragmentCore().second)) {
            change_root(id);
        }
    }
    return true;
}

bool GHSMSTAlg::handle_change_root_message(GHSMSTMessage *msg)
{
    // EV << "\t"
    //    << "GHSMSTAlg::handle_change_root_message()\n";
    int receiver_id = msg->getReceiverId();
    change_root(msg->getRealSenderId());
    return true;
}

void GHSMSTAlg::change_root(int core_node_id)
{
    // EV << "\t"
    //    << "GHSMSTAlg::change_root()\n";
    if (!is_endpoint_of_edge(best_edge_id)) {
        GHSMSTMessage *new_msg =
            create_message(GHS_MST_MESSAGE_CHANGE_ROOT, best_edge_id,
                           core_node_id, -1, id, received_best_edge_from);
        need_to_send_message_queue.insert(new_msg);
    } else {
        GHSMSTMessage *msg =
            create_message(GHS_MST_MESSAGE_CONNECT, best_edge_id);
        msg->setLevel(level);
        need_to_send_message_queue.insert(msg);
        add_branch_edge(best_edge_id);
    }
}

bool GHSMSTAlg::handle_done_message(GHSMSTMessage *msg)
{
    // EV << "\t"
    //    << "GHSMSTAlg::handle_done_message()\n";
    return true;
}

void GHSMSTAlg::print_state(int log_level)
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "state = {\n";
    s += (tab + one_tab + "id                   = " + std::to_string(id) +
          ",\n");
    s += (tab + one_tab +
          "current_round_id     = " + std::to_string(current_round_id) + ",\n");
    s += (tab + one_tab + "fragment_id          = " +
          std::to_string(fragment_core.second) + ",\n");
    s += (tab + one_tab + "fragment_core        = {" +
          tuple_to_string(fragment_core.first) + "," +
          std::to_string(fragment_core.second) + "},\n");
    s += (tab + one_tab + "level                = " + std::to_string(level) +
          ",\n");
    s += (tab + one_tab +
          "find_count           = " + std::to_string(find_count) + ",\n");
    s += (tab + one_tab + "GHS_MST_node_state   = " +
          GHSMSTNodeState_to_string(GHS_MST_node_state) + ",\n");
    s += (tab + one_tab +
          "best_edge_id         = " + tuple_to_string(best_edge_id) + "\n");
    s += (tab + one_tab +
          "test_edge_id         = " + tuple_to_string(test_edge_id) + "\n");
    s += (tab + one_tab +
          "in_branch            = " + tuple_to_string(in_branch) + "\n");

    s += (tab + one_tab + "basic_edges_set      = [\n");
    for (auto edge_id : basic_edges_set)
        s += (tab + one_tab + one_tab + tuple_to_string(edge_id) + ",\n");
    s += (tab + one_tab + "]\n");

    s += (tab + one_tab + "branch_edges_set     = [\n");
    for (auto edge_id : branch_edges_set)
        s += (tab + one_tab + one_tab + tuple_to_string(edge_id) + ",\n");
    s += (tab + one_tab + "]\n");

    s += (tab + "}");
    // EV << s << '\n';
}

GHSMSTAlg::~GHSMSTAlg() { clear_message_queue(); }