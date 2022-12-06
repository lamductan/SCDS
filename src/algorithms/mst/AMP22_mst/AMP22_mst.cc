#include "algorithms/mst/AMP22_mst/AMP22_mst.h"
#include "utils/utils.h"

void AMP22MSTAlg::set_alg_type()
{
    EV << "AMP22MSTAlg::set_alg_type()\n";
    alg_type = SLEEPING_MST;
}

AMP22MSTAlg::AMP22MSTAlg() {}

AMP22MSTAlg::AMP22MSTAlg(Node *node, int starting_round)
{
    init(node, starting_round);
}

void AMP22MSTAlg::init(Node *node, int starting_round)
{
    IAlgNode::init(node, starting_round);
    fragment_id = id;
    level = 0;
    new_fragment_id = -1;
    new_level = -1;
    log2_n = log2(n_nodes);

    next_phase_starting_round = starting_round;

    different_fragment_neighbors = neighbors_set;

    for (int neighbor_id : node->all_neighbors) {
        centralized::Edge edge(id, neighbor_id);
        std::tuple<int, int, int> edge_id = edge.get_id();
        basic_edges_set.insert(edge_id);
        edges_map[neighbor_id] = edge;
        edges_id_map[neighbor_id] = edge_id;
    }

    node_moe = fragment_moe =
        (basic_edges_set.empty()) ? INF_EDGE_ID : *basic_edges_set.begin();
}

AMP22MSTMessage *AMP22MSTAlg::create_AMP22_MST_message(int real_receiver_id)
{
    AMP22MSTMessage *AMP22_MST_message = new AMP22MSTMessage();
    AMP22_MST_message->setSenderId(id);
    AMP22_MST_message->setRealSenderId(id);
    AMP22_MST_message->setSentRoundId(current_round_id);
    AMP22_MST_message->setFragmentId(fragment_id);
    AMP22_MST_message->setLevel(level);
    AMP22_MST_message->setParent(parent);
    AMP22_MST_message->setCoinValue(coin_value);
    AMP22_MST_message->setMOE(fragment_moe);
    AMP22_MST_message->setNewFragmentId(new_fragment_id);
    AMP22_MST_message->setNewLevel(new_level);
    if (real_receiver_id != -2)
        AMP22_MST_message->setReceiverId(
            find_next_relay_node_id_on_path_to(real_receiver_id));
    AMP22_MST_message->setRealReceiverId(real_receiver_id);
    AMP22_MST_message->setRoundActionType(
        awake_rounds_in_phase[round_id_in_current_phase]);
    return AMP22_MST_message;
}

int AMP22MSTAlg::find_next_relay_node_id_on_path_to(int real_receiver_id)
{
    return real_receiver_id;
}

void AMP22MSTAlg::send_new_message(cMessage *msg, double delay)
{
    EV << "AMP22MSTAlg::send_new_message()\n";
    if (msg->getKind() != AMP22_MST_MESSAGE)
        return IAlgNode::send_new_message(msg, delay);
    AMP22MSTMessage *AMP22_MST_message = dynamic_cast<AMP22MSTMessage *>(msg);
    EV << "\tneed_to_send = " << set_to_string<int>(need_to_send) << '\n';
    for (int need_to_send_id : need_to_send) {
        AMP22MSTMessage *new_msg = AMP22_MST_message->dup();
        int real_receiver_id = new_msg->getRealReceiverId();
        if (real_receiver_id == -2) {
            new_msg->setRealReceiverId(need_to_send_id);
        }
        real_receiver_id = new_msg->getRealReceiverId();

        if (awake_rounds_in_phase[round_id_in_current_phase] ==
                AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1 &&
            coin_value == AMP22_MST_COIN_VALUE_TAIL && is_endpoint_moe &&
            new_parent == real_receiver_id)
            new_msg->setIsMSTEdge(true);

        new_msg->setReceiverId(
            find_next_relay_node_id_on_path_to(real_receiver_id));
        node->sendDelayed(new_msg, delay,
                          node->neighbor_gates[new_msg->getReceiverId()]);
        EV << "\tnew_msg->getReceiverId() = " << new_msg->getReceiverId()
           << '\n';
    }
    delete msg;
}

int AMP22MSTAlg::get_new_level(int new_parent_level) const
{
    return new_parent_level + get_round_stretch();
}

void AMP22MSTAlg::update_new_fragment_information_from_msg(
    AMP22MSTMessage *AMP22_MST_message)
{
    if (new_fragment_id != -1 || new_level != -1) return;
    new_fragment_id = AMP22_MST_message->getNewFragmentId();
    new_level = get_new_level(AMP22_MST_message->getNewLevel());
    new_parent = AMP22_MST_message->getRealSenderId();
}

void AMP22MSTAlg::print_message_queue()
{
    EV << "AMP22MSTAlg::print_message_queue()\n";
    for (cMessage *msg : message_queue) {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(msg);
        EV << AMP22_MST_message->to_string(1) << '\n';
    }
}

AMP22MSTCoinValue AMP22MSTAlg::toss_coin() const
{
    return (node->bernoulli(0.5)) ? AMP22_MST_COIN_VALUE_HEAD
                                  : AMP22_MST_COIN_VALUE_TAIL;
}

int AMP22MSTAlg::calc_n_rounds_in_current_phase() const
{
    return 4 * d + 3 + 2 * get_round_stretch();
}

int AMP22MSTAlg::calc_round_id_in_current_phase() const
{
    return (current_round_id - current_phase_starting_round) %
           n_rounds_in_current_phase;
}

int AMP22MSTAlg::get_round_stretch() const { return 1; }

void AMP22MSTAlg::add_awake_round(int round_id,
                                  AMP22MSTRoundPhaseActionType round_type)
{
    if (awake_rounds_in_phase.count(round_id) > 0) return;
    awake_rounds_in_phase[round_id] = round_type;
    awake_round_type_to_round_id[round_type] = round_id;
}

void AMP22MSTAlg::remove_awake_round(AMP22MSTRoundPhaseActionType round_type)
{
    int round_id = awake_round_type_to_round_id[round_type];
    awake_rounds_in_phase.erase(round_id);
}

void AMP22MSTAlg::reset_phase_rounds_information()
{
    // d = (d == -1) ? 1 : std::min(d * 3, n_nodes - 1);
    d = (d == -1) ? 1
                  : std::min(n_nodes - 1, 100); // It works for CDS algorithms
    // because the number of nodes in an MIS is around 400.
    // However, for general problem, it should be n_nodes - 1.
    // Maybe 200 because MST algorithm for CDS is 3-hop MST.
    ++phase_id;
    current_phase_starting_round = next_phase_starting_round;
    n_rounds_in_current_phase = calc_n_rounds_in_current_phase();
    next_phase_starting_round += n_rounds_in_current_phase;
}

void AMP22MSTAlg::reset_phase()
{
    EV << "AMP22MSTAlg::reset_phase()\n";
    reset_phase_rounds_information();
    node_moe = fragment_moe =
        (basic_edges_set.empty()) ? INF_EDGE_ID : *basic_edges_set.begin();
    is_endpoint_moe = false;
    awake_rounds_in_phase.clear();
    coin_value = AMP22_MST_COIN_VALUE_UNDEFINED;
    updated_fragment_information = false;

    new_fragment_id = -1;
    new_level = -1;
    new_parent = -2;

    // UPCAST1
    if (!is_fragment_leaf()) {
        add_awake_round(d - level - 1,
                        AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_MOE);
    }
    if (!is_fragment_root()) {
        add_awake_round(d - level,
                        AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_MOE);
    }

    // BROADCAST1
    if (!is_fragment_root()) {
        add_awake_round(
            d + level - 1,
            AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_MOE);
    }
    add_awake_round(d + level,
                    AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE);

    // INTER-FRAGMENT COMMUNICATION 1
    add_awake_round(
        2 * d,
        AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1);
    add_awake_round(
        2 * d + 1 * get_round_stretch() - 1,
        AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1_2);
    add_awake_round(
        2 * d + 1 * get_round_stretch(),
        AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_2);

    // UPCAST2
    if (!is_fragment_leaf()) {
        add_awake_round(3 * d - level + 1 * get_round_stretch(),
                        AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_TAIL);
    }
    if (!is_fragment_root()) {
        add_awake_round(3 * d - level + 1 * get_round_stretch() + 1,
                        AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL);
    }

    // BROADCAST2
    if (!is_fragment_root()) {
        add_awake_round(
            3 * d + level + 1 * get_round_stretch(),
            AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_TAIL);
    }
    add_awake_round(3 * d + level + 1 * get_round_stretch() + 1,
                    AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL);

    // INTER-FRAGMENT COMMUNICATION 2
    add_awake_round(
        4 * d + 1 * get_round_stretch() + 2,
        AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1);
    add_awake_round(
        4 * d + 2 * get_round_stretch() + 1,
        AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1_2);
    add_awake_round(
        4 * d + 2 * get_round_stretch() + 2,
        AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_2);
}

bool AMP22MSTAlg::is_fragment_leaf() const { return children.empty(); }

bool AMP22MSTAlg::is_fragment_root() const { return fragment_id == id; }

cMessage *AMP22MSTAlg::process_message_queue()
{
    if (!awake_rounds_in_phase.count(round_id_in_current_phase)) return nullptr;
    EV << "AMP22MSTAlg::process_message_queue()\n";
    print_message_queue();
    AMP22MSTMessage *need_to_send_msg = process_message_queue_1();
    if (need_to_send_msg == nullptr) return nullptr;
    EV << "need_to_send_msg = " << need_to_send_msg->to_string(0) << "\n\n";
    return need_to_send_msg;
}

AMP22MSTMessage *AMP22MSTAlg::process_message_queue_1()
{
    switch (awake_rounds_in_phase[round_id_in_current_phase]) {
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_MOE:
        return process_message_queue_UPCAST_SEND_MOE_round();
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE:
        return process_message_queue_BROADCAST_SEND_MOE_round();

    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1:
        return process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_1_round();
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1_2:
        return process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_1_2_round();
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_2:
        return process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_2_round();

    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL:
        return process_message_queue_UPCAST_SEND_TAIL_round();
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL:
        return process_message_queue_BROADCAST_SEND_TAIL_round();

    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1:
        return process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_1_round();
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1_2:
        return process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_1_2_round();
    case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_2:
        return process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_2_round();
    default:
        break;
    }
    return nullptr;
}

AMP22MSTMessage *AMP22MSTAlg::process_message_queue_UPCAST_SEND_MOE_round()
{
    EV << "AMP22MSTAlg::process_message_queue_UPCAST_SEND_MOE_round()\n";
    for (cMessage *msg : message_queue) {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(msg);
        fragment_moe = std::min(fragment_moe, AMP22_MST_message->getMOE());
    }
    AMP22MSTMessage *new_msg = create_AMP22_MST_message(parent);
    need_to_send = { parent };
    return new_msg;
}

AMP22MSTMessage *AMP22MSTAlg::process_message_queue_BROADCAST_SEND_MOE_round()
{
    EV << "AMP22MSTAlg::process_message_queue_BROADCAST_SEND_MOE_round()\n";
    if (is_fragment_root())
        return process_message_queue_BROADCAST_SEND_MOE_round_root();
    return process_message_queue_BROADCAST_SEND_MOE_round_non_root();
}

AMP22MSTMessage *
AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_1_round()
{
    EV << "AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_"
          "COMMUNICATION_1_1_round()\n";
    if (is_fragment_root() && coin_value == AMP22_MST_COIN_VALUE_UNDEFINED) {
        coin_value = toss_coin();
    } else {
        for (cMessage *msg : message_queue) {
            AMP22MSTMessage *AMP22_MST_message =
                dynamic_cast<AMP22MSTMessage *>(msg);
            fragment_moe = std::min(fragment_moe, AMP22_MST_message->getMOE());
            coin_value = AMP22_MST_message->getCoinValue();
        }
    }

    if (coin_value == AMP22_MST_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    EV << "\tcoin_value = " << AMP22MSTCoinValue_to_string(coin_value) << '\n';
    is_endpoint_moe = is_endpoint_of_edge(fragment_moe, id);
    if (coin_value == AMP22_MST_COIN_VALUE_TAIL && !is_endpoint_moe)
        return nullptr;

    AMP22MSTMessage *new_msg = nullptr;
    if (coin_value == AMP22_MST_COIN_VALUE_HEAD) {
        new_msg = create_AMP22_MST_message();
        new_msg->setNewFragmentId(fragment_id);
        new_msg->setNewLevel(level);
        need_to_send = different_fragment_neighbors;
    } else {
        new_msg =
            create_AMP22_MST_message(get_neighbor_id_of_edge(fragment_moe, id));
        need_to_send = { get_neighbor_id_of_edge(fragment_moe, id) };
    }
    return new_msg;
}

AMP22MSTMessage *
AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_1_2_round()
{
    EV << "AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_1_"
          "2_round()\n";
    return nullptr;
}

AMP22MSTMessage *
AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_2_round()
{
    EV << "AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_"
          "COMMUNICATION_1_2_round()\n";
    // This round only does local computation, so shouldn't be counted in awake
    // rounds.
    --n_awake_rounds;
    awake_round_map[current_round_id] = false;

    if (coin_value == AMP22_MST_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    // For TAIL fragment
    if (coin_value == AMP22_MST_COIN_VALUE_TAIL) {
        if (!is_endpoint_moe) return nullptr;
        int neighbor_id = get_neighbor_id_of_edge(fragment_moe, id);
        for (cMessage *msg : message_queue) {
            AMP22MSTMessage *AMP22_MST_message =
                dynamic_cast<AMP22MSTMessage *>(msg);
            int sender_id = AMP22_MST_message->getRealSenderId();
            if (sender_id != neighbor_id) continue;
            bool neighbor_coin_value = AMP22_MST_message->getCoinValue();
            if (neighbor_coin_value == AMP22_MST_COIN_VALUE_HEAD) {
                update_new_fragment_information_from_msg(AMP22_MST_message);
            }
            return nullptr;
        }
    }

    // For HEAD fragment
    for (cMessage *msg : message_queue) {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(msg);
        int sender_id = AMP22_MST_message->getRealSenderId();
        std::tuple<int, int, int> received_moe = AMP22_MST_message->getMOE();
        if (!is_endpoint_of_edge(received_moe, id)) continue;
        AMP22MSTCoinValue received_coin_value =
            AMP22_MST_message->getCoinValue();
        if (received_coin_value == AMP22_MST_COIN_VALUE_HEAD) continue;
        children.insert(sender_id);
    }

    remove_awake_round(AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_TAIL);
    remove_awake_round(AMP22_MST_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL);
    remove_awake_round(
        AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_TAIL);
    remove_awake_round(AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL);

    return nullptr;
}

AMP22MSTMessage *AMP22MSTAlg::process_message_queue_UPCAST_SEND_TAIL_round()
{
    EV << "AMP22MSTAlg::process_message_queue_UPCAST_SEND_TAIL_round()\n";
    if (coin_value == AMP22_MST_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    if (coin_value == AMP22_MST_COIN_VALUE_HEAD) return nullptr;
    if (!is_endpoint_moe && message_queue.empty()) return nullptr;
    assert(message_queue.size() <= 1);
    if (message_queue.size() == 1) {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(message_queue[0]);
        update_new_fragment_information_from_msg(AMP22_MST_message);
    }
    AMP22MSTMessage *need_to_send_msg = create_AMP22_MST_message(parent);
    need_to_send = { parent };
    return need_to_send_msg;
}

AMP22MSTMessage *AMP22MSTAlg::process_message_queue_BROADCAST_SEND_TAIL_round()
{
    EV << "AMP22MSTAlg::process_message_queue_BROADCAST_SEND_TAIL_round()"
          "\n";
    if (coin_value == AMP22_MST_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }

    if (coin_value == AMP22_MST_COIN_VALUE_HEAD) return nullptr;

    if (is_fragment_root()) {
        if (message_queue.size() == 1) {
            AMP22MSTMessage *AMP22_MST_message =
                dynamic_cast<AMP22MSTMessage *>(message_queue[0]);
            update_new_fragment_information_from_msg(AMP22_MST_message);
        }
        AMP22MSTMessage *need_to_send_msg = create_AMP22_MST_message();
        need_to_send = children;
        return need_to_send_msg;
    }

    if (message_queue.size() == 1) {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(message_queue[0]);
        update_new_fragment_information_from_msg(AMP22_MST_message);
        AMP22MSTMessage *need_to_send_msg = create_AMP22_MST_message();
        need_to_send = children;
        update_new_fragment_information_from_msg(AMP22_MST_message);
        return need_to_send_msg;
    }
    return nullptr;
}

AMP22MSTMessage *
AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_1_round()
{
    EV << "AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_"
          "COMMUNICATION_2_1_round()\n";
    // TAIL fragment nodes that can merge update fragment information
    if (new_fragment_id != -1 && new_level != -1) {
        fragment_id = new_fragment_id;
        level = new_level;
        parent = new_parent;
    }

    AMP22MSTMessage *new_msg = create_AMP22_MST_message();
    need_to_send = neighbors_set;
    return new_msg;
}

AMP22MSTMessage *
AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_1_2_round()
{
    EV << "AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_1_"
          "2_round()\n";
    return nullptr;
}

AMP22MSTMessage *
AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_2_round()
{
    EV << "AMP22MSTAlg::process_message_queue_INTER_FRAGMENT_"
          "COMMUNICATION_2_2_round()\n";
    // This round only does local computation, so shouldn't be counted in awake
    // rounds.
    --n_awake_rounds;
    awake_round_map[current_round_id] = false;

    same_fragment_neighbors.clear();
    children.clear();
    for (cMessage *msg : message_queue) {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(msg);
        int sender_id = AMP22_MST_message->getRealSenderId();
        int sender_fragment_id = AMP22_MST_message->getFragmentId();
        int sender_parent = AMP22_MST_message->getParent();
        if (sender_fragment_id != fragment_id) continue;
        same_fragment_neighbors.insert(sender_id);
        if (sender_parent == id) {
            children.insert(sender_id);
            tree_edges[sender_id] = edges_map[sender_id];
        }
    }
    tree_edges[parent] = edges_map[parent];

    different_fragment_neighbors =
        diff_set<int>(neighbors_set, same_fragment_neighbors);
    basic_edges_set.clear();
    for (int different_fragment_neighbor : different_fragment_neighbors)
        basic_edges_set.insert(edges_id_map[different_fragment_neighbor]);
    updated_fragment_information = true;
    return nullptr;
}

AMP22MSTMessage *
AMP22MSTAlg::process_message_queue_BROADCAST_SEND_MOE_round_root()
{
    EV << "AMP22MSTAlg::process_message_queue_BROADCAST_SEND_MOE_round_"
          "root()\n";
    for (cMessage *msg : message_queue) {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(msg);
        fragment_moe = std::min(fragment_moe, AMP22_MST_message->getMOE());
    }
    coin_value = toss_coin();
    AMP22MSTMessage *need_to_send_msg = create_AMP22_MST_message();
    need_to_send = children;
    return need_to_send_msg;
}

AMP22MSTMessage *
AMP22MSTAlg::process_message_queue_BROADCAST_SEND_MOE_round_non_root()
{
    EV << "AMP22MSTAlg::process_message_queue_BROADCAST_SEND_MOE_round_"
          "non_root()\n";
    assert(message_queue.size() <= 1);
    if (message_queue.size() == 1) {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(message_queue[0]);
        fragment_moe = AMP22_MST_message->getMOE();
        coin_value = AMP22_MST_message->getCoinValue();
        AMP22MSTMessage *need_to_send_msg = create_AMP22_MST_message();
        need_to_send = children;
        return need_to_send_msg;
    }
    return nullptr;
}

void AMP22MSTAlg::stage_transition()
{
    EV << "AMP22MSTAlg::stage_transition()\n";
    if (current_round_id == next_phase_starting_round) {
        reset_phase();
        if (MIS_status == IN_MIS) print_state();
    }
    round_id_in_current_phase = calc_round_id_in_current_phase();
}

bool AMP22MSTAlg::is_awake()
{
    EV << "AMP22MSTAlg::is_awake()\n";
    return (awake_rounds_in_phase.count(round_id_in_current_phase) > 0);
}

bool AMP22MSTAlg::is_decided() { return basic_edges_set.empty(); }

bool AMP22MSTAlg::is_selected() { return true; }

void AMP22MSTAlg::print_state(int log_level)
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "state = {\n";
    s += (tab + one_tab +
          "id                           = " + std::to_string(id) + ",\n");
    s += (tab + one_tab +
          "d                            = " + std::to_string(d) + ",\n");
    s += (tab + one_tab +
          "phase_id                     = " + std::to_string(phase_id) + ",\n");
    s += (tab + one_tab + "current_round_id             = " +
          std::to_string(current_round_id) + ",\n");
    s += (tab + one_tab + "current_phase_starting_round = " +
          std::to_string(current_phase_starting_round) + ",\n");
    s += (tab + one_tab + "next_phase_starting_round    = " +
          std::to_string(next_phase_starting_round) + ",\n");
    s += (tab + one_tab + "n_rounds_in_current_phase    = " +
          std::to_string(n_rounds_in_current_phase) + ",\n");
    s += (tab + one_tab + "round_id_in_current_phase    = " +
          std::to_string(round_id_in_current_phase) + ",\n");
    s += (tab + one_tab + "current_round_action         = " +
          AMP22MSTRoundPhaseActionType_to_string(
              awake_rounds_in_phase[round_id_in_current_phase]) +
          ",\n");
    s += (tab + one_tab + "awake_rounds_in_phase        = " +
          map_AMP22MSTRoundPhaseActionType_to_string(awake_rounds_in_phase,
                                                     log_level + 1, false) +
          ",\n");
    s += (tab + one_tab + "different_fragment_neighbors = " +
          set_to_string<int>(different_fragment_neighbors) + ",\n");
    s += (tab + one_tab + "same_fragment_neighbors      = " +
          set_to_string<int>(same_fragment_neighbors) + ",\n");
    s += (tab + one_tab + "basic_edges_set              = " +
          set_tuple_to_string(basic_edges_set, log_level + 1, false) + "\n");
    s += (tab + one_tab + "coin_value                   = " +
          AMP22MSTCoinValue_to_string(coin_value) + ",\n");
    s += (tab + one_tab + "fragment_id                  = " +
          std::to_string(fragment_id) + ",\n");
    s += (tab + one_tab +
          "level                        = " + std::to_string(level) + ",\n");
    s += (tab + one_tab +
          "parent                       = " + std::to_string(parent) + ",\n");
    s += (tab + one_tab + "children                     = " +
          set_to_string<int>(children) + ",\n");
    s += (tab + one_tab + "new_fragment_id              = " +
          std::to_string(new_fragment_id) + ",\n");
    s += (tab + one_tab + "new_level                    = " +
          std::to_string(new_level) + ",\n");
    s += (tab + one_tab + "new_parent                   = " +
          std::to_string(new_parent) + ",\n");
    s += (tab + one_tab + "new_chilren                  = " +
          set_to_string<int>(new_chilren) + ",\n");
    s += tab + "}";
    EV << s << '\n';
}

std::string map_AMP22MSTRoundPhaseActionType_to_string(
    const std::map<int, AMP22MSTRoundPhaseActionType> &m, int log_level,
    bool log_level_on_first_line)
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');

    std::string s = (log_level_on_first_line) ? tab + "[\n" : "[\n";

    for (const auto &it : m) {
        s += (tab + one_tab) + std::to_string(it.first) + ": " +
             AMP22MSTRoundPhaseActionType_to_string(it.second);
        s += ",\n";
    }
    s += tab + "]";
    return s;
}