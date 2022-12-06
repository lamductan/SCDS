#include "algorithms/cds/Lam23_cds_local/Lam23_BFS_part2.h"
#include "utils/utils.h"

void LamBFSPart2Alg::set_alg_type()
{
    EV << "LamBFSPart2Alg::set_alg_type()\n";
    alg_type = BFS_ALG;
}

LamBFSPart2Alg::LamBFSPart2Alg() {}

void LamBFSPart2Alg::init(Node *node, int starting_round)
{
    LamBFSPart1Alg::init(node, starting_round);
    previous_fragment_status = fragment_information.fragment_status = true;
    log2_n = log2(n_nodes);
    next_stage_starting_round = starting_round;

    max_num_rounds = 28 * 10; // TODO: for DEBUG purpose only
}

void LamBFSPart2Alg::init_from_Lam_BFS_part1_alg(
    LamBFSPart1Alg *Lam_BFS_part1_alg)
{
    starting_round = Lam_BFS_part1_alg->max_num_rounds + 1;
    IAlgNode::init(Lam_BFS_part1_alg->node, starting_round);
    log2_n = log2(n_nodes);

    fragment_information = Lam_BFS_part1_alg->fragment_information;
    previous_fragment_status = fragment_information.fragment_status = true;

    basic_edges_set = Lam_BFS_part1_alg->basic_edges_set;
    different_fragment_neighbors =
        Lam_BFS_part1_alg->different_fragment_neighbors;
    bfs_tree_structure = Lam_BFS_part1_alg->bfs_tree_structure;

    next_stage_starting_round = starting_round;

    max_num_rounds = 185; // TODO: for DEBUG purpose only
}

LamBFSPart2Alg::LamBFSPart2Alg(Node *node, int starting_round)
{
    init(node, starting_round);
}

int LamBFSPart2Alg::calc_next_stage_starting_round() const
{
    return starting_round + 2 * log2_n * (12 * d + 4 * (log2_d + 1) - 6);
}

int LamBFSPart2Alg::calc_n_rounds_in_phase_this_stage() const
{
    return 6 * d + 4;
}

int LamBFSPart2Alg::calc_round_id_in_current_phase() const
{
    return (current_round_id - current_stage_starting_round) %
           n_rounds_in_phase_this_stage;
}

void LamBFSPart2Alg::reset_stage()
{
    d = (d == -1) ? 1 : (d * 4); // TODO: switch to *2
    ++log2_d;
    current_stage_starting_round = next_stage_starting_round;
    next_phase_starting_round = next_stage_starting_round;
    next_stage_starting_round = calc_next_stage_starting_round();
    n_rounds_in_phase_this_stage = calc_n_rounds_in_phase_this_stage();
}

void LamBFSPart2Alg::reset_phase()
{
    EV << "LamBFSPart2Alg::reset_phase()\n";
    if (!updated_fragment_information_in_current_phase)
        update_fragment_information();

    Lam_bfs_part2_stage = LamBFSPart2Stage::FIND_MOE;
    next_phase_starting_round += n_rounds_in_phase_this_stage;
    previous_fragment_status = fragment_information.fragment_status = true;
    node_moe = fragment_moe =
        (basic_edges_set.empty()) ? INF_EDGE_ID : *basic_edges_set.begin();
    can_merge = false;
    is_endpoint_moe = false;
    awake_rounds_in_phase.clear();
    new_fragment_information = fragment_information;
    tail_fragment_nodes.clear();
    coin_value = LAM_BFS_COIN_VALUE_UNDEFINED;
    ++phase_id;
    updated_fragment_information_in_current_phase = false;

    int l = bfs_tree_structure.level;
    // UPCAST1
    if (!is_fragment_leaf()) {
        awake_rounds_in_phase[d - l - 1] =
            LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_MOE;
    }
    if (!is_fragment_root()) {
        awake_rounds_in_phase[d - l] =
            LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_MOE;
    }

    // BROADCAST1
    if (!is_fragment_root()) {
        awake_rounds_in_phase[d + l - 1] =
            LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_MOE;
    }

    awake_rounds_in_phase[d + l] =
        LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE;

    // INTER-FRAGMENT COMMUNICATION 1
    awake_rounds_in_phase[2 * d] =
        LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1;
    awake_rounds_in_phase[2 * d + 1] =
        LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_2;

    // UPCAST2
    if (!is_fragment_leaf()) {
        awake_rounds_in_phase[3 * d - l + 1] =
            LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_HEAD;
    }
    if (!is_fragment_root()) {
        awake_rounds_in_phase[3 * d - l + 2] =
            LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_HEAD;
    }

    // BROADCAST2
    if (!is_fragment_root()) {
        awake_rounds_in_phase[3 * d + l + 1] =
            LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_HEAD;
    }

    awake_rounds_in_phase[3 * d + l + 2] =
        LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_HEAD;

    // INTER-FRAGMENT COMMUNICATION 2
    awake_rounds_in_phase[4 * d + 2] =
        LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1;
    awake_rounds_in_phase[4 * d + 3] =
        LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_2;

    // UPCAST3
    if (!is_fragment_leaf()) {
        awake_rounds_in_phase[5 * d - l + 3] =
            LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_TAIL;
    }
    if (!is_fragment_root()) {
        awake_rounds_in_phase[5 * d - l + 4] =
            LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL;
    }

    // BROADCAST3
    if (!is_fragment_root()) {
        awake_rounds_in_phase[5 * d + l + 3] =
            LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_TAIL;
    }

    awake_rounds_in_phase[5 * d + l + 4] =
        LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL;
}

bool LamBFSPart2Alg::is_fragment_leaf() const
{
    return bfs_tree_structure.children.empty();
}

bool LamBFSPart2Alg::is_fragment_root() const
{
    return fragment_information.fragment_id == id;
}

cMessage *LamBFSPart2Alg::process_message_queue()
{
    if (!awake_rounds_in_phase.count(round_id_in_current_phase)) return nullptr;
    EV << "LamBFSPart2Alg::process_message_queue()\n";
    print_message_queue();
    LamBFSMessage *need_to_send_msg = process_message_queue_1();
    if (need_to_send_msg == nullptr) return nullptr;
    EV << "need_to_send_msg = " << need_to_send_msg->to_string(0) << "\n\n";
    return need_to_send_msg;
}

LamBFSMessage *LamBFSPart2Alg::process_message_queue_1()
{
    switch (awake_rounds_in_phase[round_id_in_current_phase]) {
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_MOE:
        return process_message_queue_UPCAST_SEND_MOE_round();
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE:
        return process_message_queue_BROADCAST_SEND_MOE_round();
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1:
        return process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_1_round();
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_2:
        return process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_2_round();
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_HEAD:
        return process_message_queue_UPCAST_SEND_HEAD_round();
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_HEAD:
        return process_message_queue_BROADCAST_SEND_HEAD_round();
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1:
        return process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_1_round();
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_2:
        return process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_2_round();
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL:
        return process_message_queue_UPCAST_SEND_TAIL_round();
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL:
        return process_message_queue_BROADCAST_SEND_TAIL_round();
    default:
        break;
    }
    return nullptr;
}

LamBFSMessage *LamBFSPart2Alg::process_message_queue_UPCAST_SEND_MOE_round()
{
    EV << "LamBFSPart2Alg::process_message_queue_UPCAST_SEND_MOE_round()\n";
    for (cMessage *msg : message_queue) {
        LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
        fragment_moe = std::min(fragment_moe, Lam_bfs_message->getMOE());
    }
    LamBFSMessage *new_msg = create_Lam_bfs_message();
    need_to_send = { bfs_tree_structure.parent };
    return new_msg;
}

LamBFSMessage *LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_MOE_round()
{
    EV << "LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_MOE_round()\n";
    if (is_fragment_root())
        return process_message_queue_BROADCAST_SEND_MOE_round_root();
    return process_message_queue_BROADCAST_SEND_MOE_round_non_root();
}

LamBFSMessage *
LamBFSPart2Alg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_1_round()
{
    EV << "LamBFSPart2Alg::process_message_queue_INTER_FRAGMENT_"
          "COMMUNICATION_1_1_round()\n";
    if (fragment_information.fragment_diameter == 0) {
        coin_value = toss_coin();
    } else {
        for (cMessage *msg : message_queue) {
            LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
            fragment_moe = std::min(fragment_moe, Lam_bfs_message->getMOE());
            coin_value = Lam_bfs_message->getCoinValue();
        }
    }

    if (coin_value == LAM_BFS_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    EV << "\tcoin_value = " << LamBFSCoinValue_to_string(coin_value) << '\n';
    is_endpoint_moe = is_endpoint_of_edge(fragment_moe, id);
    if (coin_value == LAM_BFS_COIN_VALUE_TAIL && !is_endpoint_moe)
        return nullptr;

    LamBFSMessage *new_msg = create_Lam_bfs_message();
    if (coin_value == LAM_BFS_COIN_VALUE_HEAD) {
        need_to_send = different_fragment_neighbors;
    } else {
        need_to_send = { get_neighbor_id_of_edge(fragment_moe, id) };
    }
    return new_msg;
}

LamBFSMessage *
LamBFSPart2Alg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_1_2_round()
{
    EV << "LamBFSPart2Alg::process_message_queue_INTER_FRAGMENT_"
          "COMMUNICATION_1_2_round()\n";
    if (coin_value == LAM_BFS_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    // For TAIL fragment
    if (coin_value == LAM_BFS_COIN_VALUE_TAIL) {
        if (!is_endpoint_moe) return nullptr;
        int neighbor_id = get_neighbor_id_of_edge(fragment_moe, id);
        for (cMessage *msg : message_queue) {
            LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
            int sender_id = Lam_bfs_message->getSenderId();
            if (sender_id != neighbor_id) continue;
            bool neighbor_coin_value = Lam_bfs_message->getCoinValue();
            if (neighbor_coin_value == LAM_BFS_COIN_VALUE_HEAD) {
                can_merge = true;
            }
            return nullptr;
        }
    }

    // For HEAD fragment
    for (cMessage *msg : message_queue) {
        LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
        int sender_id = Lam_bfs_message->getSenderId();
        std::tuple<int, int, int> received_moe = Lam_bfs_message->getMOE();
        if (!is_endpoint_of_edge(received_moe, id)) continue;
        LamBFSCoinValue received_coin_value = Lam_bfs_message->getCoinValue();
        if (received_coin_value == LAM_BFS_COIN_VALUE_HEAD) continue;
        update_new_fragment_information_from_msg(Lam_bfs_message);
        tail_fragment_nodes.insert(sender_id);
    }
    return nullptr;
}

LamBFSMessage *LamBFSPart2Alg::process_message_queue_UPCAST_SEND_HEAD_round()
{
    EV << "LamBFSPart2Alg::process_message_queue_UPCAST_SEND_HEAD_round()\n";
    if (coin_value == LAM_BFS_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    if (coin_value == LAM_BFS_COIN_VALUE_TAIL) return nullptr;
    for (cMessage *msg : message_queue) {
        LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
        update_new_fragment_information_from_msg(Lam_bfs_message);
    }
    need_to_send = { bfs_tree_structure.parent };
    return create_Lam_bfs_message();
}

LamBFSMessage *LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_HEAD_round()
{
    EV << "LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_HEAD_round()\n";
    if (coin_value == LAM_BFS_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    if (coin_value == LAM_BFS_COIN_VALUE_TAIL) return nullptr;
    if (is_fragment_root())
        return process_message_queue_BROADCAST_SEND_HEAD_round_root();
    return process_message_queue_BROADCAST_SEND_HEAD_round_non_root();
}

LamBFSMessage *
LamBFSPart2Alg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_1_round()
{
    EV << "LamBFSPart2Alg::process_message_queue_INTER_FRAGMENT_"
          "COMMUNICATION_2_1_round()\n";
    if (coin_value == LAM_BFS_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    if (coin_value == LAM_BFS_COIN_VALUE_TAIL) return nullptr;

    // Node at level d of Head fragment process message
    if (fragment_information.fragment_diameter == 0) {
        fragment_root_compute_new_bfs_tree_structure();
    } else {
        assert(message_queue.size() <= 1);
        if (message_queue.size() == 1) {
            LamBFSMessage *Lam_bfs_message =
                dynamic_cast<LamBFSMessage *>(message_queue[0]);
            new_fragment_information =
                Lam_bfs_message->getNewFragmentInformation();
        }
    }

    // HEAD send
    if (tail_fragment_nodes.empty()) return nullptr;
    LamBFSMessage *need_to_send_msg = create_Lam_bfs_message();
    need_to_send = tail_fragment_nodes;
    update_fragment_information(); // HEAD update after inter-fragment
    return need_to_send_msg;
}

LamBFSMessage *
LamBFSPart2Alg::process_message_queue_INTER_FRAGMENT_COMMUNICATION_2_2_round()
{
    EV << "LamBFSPart2Alg::process_message_queue_INTER_FRAGMENT_"
          "COMMUNICATION_2_2_round()\n";
    if (coin_value == LAM_BFS_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    // TAIL receive
    if (coin_value == LAM_BFS_COIN_VALUE_HEAD) return nullptr;
    if (!is_endpoint_moe || !can_merge) return nullptr;
    assert(message_queue.size() <= 1);
    if (message_queue.size() == 1) {
        LamBFSMessage *Lam_bfs_message =
            dynamic_cast<LamBFSMessage *>(message_queue[0]);
        new_fragment_information = Lam_bfs_message->getNewFragmentInformation();
    }
    if (fragment_information.fragment_diameter == 0) {
        update_fragment_information();
    }
    return nullptr;
}

LamBFSMessage *LamBFSPart2Alg::process_message_queue_UPCAST_SEND_TAIL_round()
{
    EV << "LamBFSPart2Alg::process_message_queue_UPCAST_SEND_TAIL_round()\n";
    if (coin_value == LAM_BFS_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    if (coin_value == LAM_BFS_COIN_VALUE_HEAD) return nullptr;
    if (!is_endpoint_moe && message_queue.empty()) return nullptr;
    assert(message_queue.size() <= 1);
    if (message_queue.size() == 1) {
        LamBFSMessage *Lam_bfs_message =
            dynamic_cast<LamBFSMessage *>(message_queue[0]);
        new_fragment_information = Lam_bfs_message->getNewFragmentInformation();
    }
    LamBFSMessage *need_to_send_msg = create_Lam_bfs_message();
    need_to_send = { bfs_tree_structure.parent };
    return need_to_send_msg;
}

LamBFSMessage *LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_TAIL_round()
{
    EV << "LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_TAIL_round()\n";
    if (coin_value == LAM_BFS_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }

    if (coin_value == LAM_BFS_COIN_VALUE_HEAD) return nullptr;

    if (is_fragment_root()) {
        if (message_queue.size() == 1) {
            LamBFSMessage *Lam_bfs_message =
                dynamic_cast<LamBFSMessage *>(message_queue[0]);
            new_fragment_information =
                Lam_bfs_message->getNewFragmentInformation();
        }
        LamBFSMessage *need_to_send_msg = create_Lam_bfs_message();
        need_to_send = bfs_tree_structure.children;
        return need_to_send_msg;
    }

    for (cMessage *msg : message_queue) {
        LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
        int sender_id = Lam_bfs_message->getSenderId();
        if (sender_id != bfs_tree_structure.parent) continue;
        new_fragment_information = Lam_bfs_message->getNewFragmentInformation();
        LamBFSMessage *need_to_send_msg = create_Lam_bfs_message();
        need_to_send = bfs_tree_structure.children;
        update_fragment_information();
        return need_to_send_msg;
    }
    return nullptr;
}

LamBFSMessage *
LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_MOE_round_root()
{
    EV << "LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_MOE_round_"
          "root()\n";
    for (cMessage *msg : message_queue) {
        LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
        fragment_moe = std::min(fragment_moe, Lam_bfs_message->getMOE());
    }
    coin_value = toss_coin();
    LamBFSMessage *need_to_send_msg = create_Lam_bfs_message();
    need_to_send = bfs_tree_structure.children;
    return need_to_send_msg;
}

LamBFSMessage *
LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_MOE_round_non_root()
{
    EV << "LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_MOE_round_"
          "non_root()\n";
    assert(message_queue.size() <= 1);
    if (message_queue.size() == 1) {
        LamBFSMessage *Lam_bfs_message =
            dynamic_cast<LamBFSMessage *>(message_queue[0]);
        fragment_moe = Lam_bfs_message->getMOE();
        coin_value = Lam_bfs_message->getCoinValue();
        LamBFSMessage *need_to_send_msg = Lam_bfs_message->dup();
        need_to_send_msg->setSenderId(id);
        need_to_send = bfs_tree_structure.children;
        return need_to_send_msg;
    }
    return nullptr;
}

LamBFSMessage *
LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_HEAD_round_root()
{
    EV << "LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_HEAD_round_"
          "root()\n";
    if (coin_value == LAM_BFS_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    if (coin_value == LAM_BFS_COIN_VALUE_TAIL) return nullptr;
    for (cMessage *msg : message_queue) {
        LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
        int sender_id = Lam_bfs_message->getSenderId();
        LamBFSCoinValue neighbor_coin_value = Lam_bfs_message->getCoinValue();
        if (neighbor_coin_value == LAM_BFS_COIN_VALUE_TAIL) continue;
        update_new_fragment_information_from_msg(Lam_bfs_message);
    }

    fragment_root_compute_new_bfs_tree_structure();
    LamBFSMessage *Lam_bfs_message = create_Lam_bfs_message();
    need_to_send = bfs_tree_structure.children;
    return Lam_bfs_message;
}

LamBFSMessage *
LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_HEAD_round_non_root()
{
    EV << "LamBFSPart2Alg::process_message_queue_BROADCAST_SEND_HEAD_round_"
          "non_root()";
    if (coin_value == LAM_BFS_COIN_VALUE_UNDEFINED) {
        fprintf(stderr, "coin_value == UNDEFINED\n");
        exit(1);
    }
    if (coin_value == LAM_BFS_COIN_VALUE_TAIL) return nullptr;
    for (cMessage *msg : message_queue) {
        LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
        int sender_id = Lam_bfs_message->getSenderId();
        if (sender_id != bfs_tree_structure.parent) continue;
        new_fragment_information = Lam_bfs_message->getNewFragmentInformation();
        LamBFSMessage *need_to_send_msg = Lam_bfs_message->dup();
        need_to_send_msg->setSenderId(id);
        need_to_send = bfs_tree_structure.children;
        return need_to_send_msg;
    }
    return nullptr;
}

void LamBFSPart2Alg::stage_transition()
{
    EV << "LamBFSPart2Alg::stage_transition()\n";
    if (current_round_id == next_stage_starting_round) {
        reset_stage();
    }
    if (current_round_id == next_phase_starting_round) {
        reset_phase();
    }

    if (previous_fragment_status == fragment_information.fragment_status ==
        false) {
        Lam_bfs_part2_stage = LamBFSPart2Stage::WAITING_FOR_NEXT_STAGE;
        return;
    }

    if (Lam_bfs_part2_stage == LamBFSPart2Stage::WAITING_FOR_NEXT_STAGE) return;

    round_id_in_current_phase = calc_round_id_in_current_phase();
    if (0 <= round_id_in_current_phase < 2 * d) {
        Lam_bfs_part2_stage = LamBFSPart2Stage::FIND_MOE;
    } else if (2 * d <= round_id_in_current_phase <
               n_rounds_in_phase_this_stage) {
        Lam_bfs_part2_stage = LamBFSPart2Stage::MERGE;
    }

    previous_fragment_status = fragment_information.fragment_status;
}

bool LamBFSPart2Alg::is_awake()
{
    EV << "LamBFSPart2Alg::is_awake()\n";
    switch (Lam_bfs_part2_stage) {
    case LamBFSPart2Stage::FIND_MOE:
    case LamBFSPart2Stage::MERGE:
        if (awake_rounds_in_phase.count(round_id_in_current_phase) > 0)
            return true;
        break;
    case LamBFSPart2Stage::WAITING_FOR_NEXT_STAGE:
        return false;
    default:
        return false;
    }
    return false;
}

bool LamBFSPart2Alg::is_decided()
{
    return fragment_information.fragment_nodes.size() == n_nodes;
}

bool LamBFSPart2Alg::is_selected() { return true; }

centralized::BFSTreeStructure LamBFSPart2Alg::get_bfs_tree_structure() const
{
    return bfs_tree_structure;
}

void LamBFSPart2Alg::print_state(int log_level)
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
    s += (tab + one_tab + "current_stage_starting_round = " +
          std::to_string(current_stage_starting_round) + ",\n");
    s += (tab + one_tab + "next_stage_starting_round    = " +
          std::to_string(next_stage_starting_round) + ",\n");
    s += (tab + one_tab + "n_rounds_in_phase_this_stage = " +
          std::to_string(n_rounds_in_phase_this_stage) + ",\n");
    s += (tab + one_tab + "next_phase_starting_round    = " +
          std::to_string(next_phase_starting_round) + ",\n");
    s += (tab + one_tab + "round_id_in_current_phase    = " +
          std::to_string(round_id_in_current_phase) + ",\n");
    s += (tab + one_tab + "awake_rounds_in_phase        = " +
          map_LamBFSRoundPhaseActionType_to_string(awake_rounds_in_phase,
                                                   log_level + 1, false) +
          ",\n");
    s += (tab + one_tab + "fragment_information         = " +
          fragment_information.to_string(log_level + 1, false) + ",\n");
    s += (tab + one_tab + "new_fragment_information     = " +
          new_fragment_information.to_string(log_level + 1, false) + ",\n");
    s += (tab + one_tab + "different_fragment_neighbors = " +
          set_to_string<int>(different_fragment_neighbors) + ",\n");
    s += (tab + one_tab + "basic_edges_set              = " +
          set_tuple_to_string(basic_edges_set, log_level + 1, false) + "\n");
    s += (tab + one_tab + "coin_value                   = " +
          LamBFSCoinValue_to_string(coin_value) + ",\n");
    s += (tab + one_tab + "bfs_tree_structure           = " +
          bfs_tree_structure.to_string(log_level + 1, false) + ",\n");
    s += tab + "}";
    EV << s << '\n';
}

std::string LamBFSRoundPhaseActionType_to_string(
    LamBFSRoundPhaseActionType Lam_bfs_round_phase_action_type)
{
    switch (Lam_bfs_round_phase_action_type) {
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_MOE:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_MOE";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_MOE:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_MOE";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_MOE:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_MOE";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE";

    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_1:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_"
               "COMMUNICATION_1_1";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_1_2:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_"
               "COMMUNICATION_1_2";

    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_HEAD:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_HEAD";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_HEAD:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_HEAD";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_HEAD:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_HEAD";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_HEAD:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_HEAD";

    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_"
               "COMMUNICATION_2_1";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_2:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_"
               "COMMUNICATION_2_2";

    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_TAIL:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_RECEIVE_TAIL";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_UPCAST_SEND_TAIL";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_TAIL:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_RECEIVE_TAIL";
    case LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL:
        return "LAM_BFS_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_TAIL";
    }
    return "";
}

std::string map_LamBFSRoundPhaseActionType_to_string(
    const std::map<int, LamBFSRoundPhaseActionType> &m, int log_level,
    bool log_level_on_first_line)
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');

    std::string s = (log_level_on_first_line) ? tab + "[\n" : "[\n";

    for (const auto &it : m) {
        s += (tab + one_tab) + std::to_string(it.first) + ": " +
             LamBFSRoundPhaseActionType_to_string(it.second);
        s += ",\n";
    }
    s += tab + "]";
    return s;
}