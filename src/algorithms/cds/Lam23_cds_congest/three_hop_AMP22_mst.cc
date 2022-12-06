#include "algorithms/cds/Lam23_cds_congest/three_hop_AMP22_mst.h"
#include "utils/utils.h"

ThreeHopAMP22MSTAlg::ThreeHopAMP22MSTAlg(Node *node, int starting_round)
{
    init(node, starting_round);
}

void ThreeHopAMP22MSTAlg::init(Node *node, int starting_round)
{
    IAlgNode::init(node, starting_round);
    fragment_id = id;
    level = 0;
    new_fragment_id = -1;
    new_level = -1;
    log2_n = log2(n_nodes);

    next_phase_starting_round = starting_round;
}

int ThreeHopAMP22MSTAlg::get_round_stretch() const { return 3; }

void ThreeHopAMP22MSTAlg::reset_phase()
{
    if (MIS_status == IN_MIS) return AMP22MSTAlg::reset_phase();
    EV << "ThreeHopAMP22MSTAlg::reset_phase()\n";
    reset_phase_rounds_information();

    for (auto it : pos_of_relay_on_directed_paths) {
        std::tuple<int, int, int> directed_edge = it.first;
        int u = std::get<1>(directed_edge);
        int v = std::get<2>(directed_edge);
        if (fragment_informations.at(u).fragment_id ==
            fragment_informations.at(v).fragment_id) {
            inter_fragment_directed_edges.erase(directed_edge);
            intra_fragment_directed_edges.insert(directed_edge);
        }
    }

    awake_rounds_in_phase_set = calculate_awake_rounds_in_phase();
    EV << "\tawake_rounds_in_phase_set = "
       << set_to_string<int>(awake_rounds_in_phase_set) << '\n';
    EV << "\ton_w_2_path = " << on_w_2_path << ' '
       << tuple_to_string(pos_of_relay_on_directed_paths.rbegin()->first)
       << '\n';
}

int ThreeHopAMP22MSTAlg::find_next_relay_node_id_on_path_to(
    int real_receiver_id)
{
    return nodes_on_path[real_receiver_id][1];
}

void ThreeHopAMP22MSTAlg::print_message_set() const
{
    EV << "ThreeHopAMP22MSTAlg::print_message_set()\n";
    for (cMessage *msg : message_set) {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(msg);
        EV << AMP22_MST_message->to_string(1) << '\n';
    }
}

std::multiset<int> ThreeHopAMP22MSTAlg::calculate_awake_rounds_in_phase() const
{
    EV << "ThreeHopAMP22MSTAlg::calculate_awake_rounds_in_phase()\n";
    std::multiset<int> awake_rounds_set_1;
    for (const auto &it : pos_of_relay_on_directed_paths) {
        std::tuple<int, int, int> directed_edge = it.first;
        EV << "\tEdge: " << tuple_to_string(directed_edge) << '\n';
        int pos = it.second;
        awake_rounds_set_1 = combine_set<int>(
            awake_rounds_set_1,
            calculate_awake_rounds_in_phase_for_directed_edge_inter_fragment_2(
                directed_edge, pos));
        awake_rounds_set_1 =
            combine_set<int>(awake_rounds_set_1,
                             calculate_awake_rounds_in_phase_for_directed_edge(
                                 directed_edge, pos));
    }
    return awake_rounds_set_1;
}

std::set<int> ThreeHopAMP22MSTAlg::
    calculate_awake_rounds_in_phase_for_directed_edge_inter_fragment_2(
        const std::tuple<int, int, int> &directed_edge, int pos) const
{
    EV << "ThreeHopAMP22MSTAlg::calculate_awake_rounds_in_phase_for_directed_"
          "edge_inter_fragment_2()\n";
    std::set<int> awake_rounds_set_1;
    EV << "\tEdge " << tuple_to_string(directed_edge) << ' ' << pos << '\n';
    int w = std::get<0>(directed_edge);
    int redundant_round = get_redundant_round(w, pos);
    int r = 4 * d + 1 * get_round_stretch() + 2;
    for (int i = 0; i < get_round_stretch(); ++i) {
        if (i == redundant_round) continue;
        awake_rounds_set_1.insert(r + i);
    }
    EV << "\t\tInter-fragment2: " << set_to_string<int>(awake_rounds_set_1)
       << '\n';
    return awake_rounds_set_1;
}

std::set<int>
ThreeHopAMP22MSTAlg::calculate_awake_rounds_in_phase_for_directed_edge(
    const std::tuple<int, int, int> &directed_edge, int pos) const
{
    if (intra_fragment_directed_edges.count(directed_edge) > 0)
        return calculate_awake_rounds_in_phase_for_directed_edge_same_fragment(
            directed_edge, pos);
    return calculate_awake_rounds_in_phase_for_directed_edge_different_fragment(
        directed_edge, pos);
}

std::set<int> ThreeHopAMP22MSTAlg::
    calculate_awake_rounds_in_phase_for_directed_edge_same_fragment(
        const std::tuple<int, int, int> &directed_edge, int pos,
        bool stage_for_tail_fragment_only) const
{
    EV << "ThreeHopAMP22MSTAlg::calculate_awake_rounds_in_phase_for_directed_"
          "edge_same_fragment()\n";
    int u = std::get<1>(directed_edge);
    int v = std::get<2>(directed_edge);
    int w = std::get<0>(directed_edge);

    FragmentInformation fragment_information_u = fragment_informations.at(u);
    int parent_u = fragment_information_u.parent;
    int level_u = fragment_information_u.level;

    FragmentInformation fragment_information_v = fragment_informations.at(v);
    int parent_v = fragment_information_v.parent;
    int level_v = fragment_information_v.level;

    if (parent_u == v) {
        return calculate_awake_rounds_in_phase_for_directed_edge_child_to_parent(
            level_u, level_v, w, pos, stage_for_tail_fragment_only);
    } else if (parent_v == u) {
        return calculate_awake_rounds_in_phase_for_directed_edge_parent_to_child(
            level_u, level_v, w, pos, stage_for_tail_fragment_only);
    }
    return std::set<int>();
}

int ThreeHopAMP22MSTAlg::get_redundant_round(int w, int pos)
{
    if (pos == 1 && w == 1) return 1;
    if (pos == 1 && w == 2) return 2;
    return 0;
}

std::set<int> ThreeHopAMP22MSTAlg::
    calculate_awake_rounds_in_phase_for_directed_edge_child_to_parent(
        int level_child, int level_parent, int w, int pos,
        bool stage_for_tail_fragment_only) const
{
    std::set<int> awake_rounds_set_1;
    int redundant_round = get_redundant_round(w, pos);

    if (!stage_for_tail_fragment_only) {
        int r = d - level_child;
        for (int i = 0; i < get_round_stretch(); ++i) {
            if (i == redundant_round) continue;
            awake_rounds_set_1.insert(r + i);
        }
    }

    int r = 3 * d - level_child + 1 * get_round_stretch() + 1;
    for (int i = 0; i < get_round_stretch(); ++i) {
        if (i == redundant_round) continue;
        awake_rounds_set_1.insert(r + i);
    }

    EV << "\t\tchild_to_parent: " << set_to_string<int>(awake_rounds_set_1)
       << '\n';
    return awake_rounds_set_1;
}

std::set<int> ThreeHopAMP22MSTAlg::
    calculate_awake_rounds_in_phase_for_directed_edge_parent_to_child(
        int level_parent, int level_child, int w, int pos,
        bool stage_for_tail_fragment_only) const
{
    std::set<int> awake_rounds_set_1;
    int redundant_round = get_redundant_round(w, pos);

    if (!stage_for_tail_fragment_only) {
        int r = d + level_parent;
        for (int i = 0; i < get_round_stretch(); ++i) {
            if (i == redundant_round) continue;
            awake_rounds_set_1.insert(r + i);
        }
    }

    int r = 3 * d + level_parent + 1 * get_round_stretch() + 1;
    for (int i = 0; i < get_round_stretch(); ++i) {
        if (i == redundant_round) continue;
        awake_rounds_set_1.insert(r + i);
    }

    EV << "\t\tparent_to_child: " << set_to_string<int>(awake_rounds_set_1)
       << '\n';
    return awake_rounds_set_1;
}

std::set<int> ThreeHopAMP22MSTAlg::
    calculate_awake_rounds_in_phase_for_directed_edge_different_fragment(
        const std::tuple<int, int, int> &directed_edge, int pos) const
{
    EV << "ThreeHopAMP22MSTAlg::calculate_awake_rounds_in_phase_for_directed_"
          "edge_different_fragment()\n";
    std::set<int> awake_rounds_set_1;
    int w = std::get<0>(directed_edge);
    int redundant_round = get_redundant_round(w, pos);
    int r = 2 * d;
    for (int i = 0; i < get_round_stretch(); ++i) {
        if (i == redundant_round) continue;
        awake_rounds_set_1.insert(r + i);
    }
    EV << "\t\tInter-fragment1: " << set_to_string<int>(awake_rounds_set_1)
       << '\n';
    return awake_rounds_set_1;
}

void ThreeHopAMP22MSTAlg::
    remove_head_fragment_awake_rounds_in_stage_for_tail_fragment()
{
    if (intra_fragment_directed_edges.empty()) return;
    EV << "ThreeHopAMP22MSTAlg::remove_head_fragment_awake_rounds_in_stage_"
          "for_tail_fragment()\n";
    EV << "\tawake_rounds_in_phase_set_before = "
       << set_to_string<int>(awake_rounds_in_phase_set) << '\n';
    for (const std::tuple<int, int, int> &directed_edge :
         intra_fragment_directed_edges) {
        int u = std::get<1>(directed_edge);
        int v = std::get<2>(directed_edge);
        const FragmentInformation &fragment_information_u =
            fragment_informations.at(u);
        const FragmentInformation &fragment_information_v =
            fragment_informations.at(v);
        EV << "fragment_information_u.coin_value = "
           << AMP22MSTCoinValue_to_string(fragment_information_u.coin_value)
           << '\n';
        EV << "fragment_information_v.coin_value = "
           << AMP22MSTCoinValue_to_string(fragment_information_v.coin_value)
           << '\n';
        EV << "u = " << u << ", v = " << v << '\n';
        if (fragment_information_u.coin_value == AMP22_MST_COIN_VALUE_TAIL ||
            fragment_information_v.coin_value == AMP22_MST_COIN_VALUE_TAIL)
            continue;
        EV << "\tHEAD fragment edge\n";
        std::set<int> awake_round_stage_for_tail =
            calculate_awake_rounds_in_phase_for_directed_edge_same_fragment(
                directed_edge, pos_of_relay_on_directed_paths[directed_edge],
                true);
        awake_rounds_in_phase_set = diff_set<int>(awake_rounds_in_phase_set,
                                                  awake_round_stage_for_tail);
    }
    EV << "\tawake_rounds_in_phase_set_after = "
       << set_to_string<int>(awake_rounds_in_phase_set) << '\n';
}

void ThreeHopAMP22MSTAlg::send_new_message(cMessage *msg, double delay)
{
    if (MIS_status == IN_MIS) return AMP22MSTAlg::send_new_message(msg, delay);
    AMP22MSTMessage *AMP22_MST_message = dynamic_cast<AMP22MSTMessage *>(msg);
    node->sendDelayed(AMP22_MST_message, delay,
                      node->neighbor_gates[AMP22_MST_message->getReceiverId()]);
}

void ThreeHopAMP22MSTAlg::process_round()
{
    if (MIS_status == IN_MIS) return IAlgNode::process_round();
    stage_transition();
    if (round_id_in_current_phase == 2 * d + 1 * get_round_stretch()) {
        remove_head_fragment_awake_rounds_in_stage_for_tail_fragment();
    }

    EV << "ThreeHopAMP22MSTAlg::process_round()\n";
    if (!is_awake()) {
        EV << "not awake\n";
        return;
    }
    ++n_awake_rounds;
    awake_round_map[current_round_id] = true;
    EV << "Is awake: n_awake_rounds = " << n_awake_rounds << '\n';
    EV << "pre_process: message_set.size() = " << message_set.size() << '\n';
    print_message_set();

    std::vector<AMP22MSTMessage *> processed_msg;
    for (cMessage *msg : message_set) {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(msg);
        int receiver_id = find_next_relay_node_id_on_path_to(
            AMP22_MST_message->getRealReceiverId());
        int real_receiver_id = AMP22_MST_message->getRealReceiverId();
        if (receiver_id == real_receiver_id) {
            int sent_round_id = AMP22_MST_message->getSentRoundId();
            if (sent_round_id + 2 > current_round_id) continue;
        }

        AMP22MSTRoundPhaseActionType round_action_type =
            AMP22_MST_message->getRoundActionType();
        int real_sender_id = AMP22_MST_message->getRealSenderId();
        FragmentInformation &sender_fragment_information =
            fragment_informations[real_sender_id];
        switch (round_action_type) {
        case AMP22_MST_ROUND_PHASE_ACTION_TYPE_BROADCAST_SEND_MOE: {
            AMP22MSTCoinValue coin_value = AMP22_MST_message->getCoinValue();
            FragmentInformation &receiver_fragment_information =
                fragment_informations[real_receiver_id];
            sender_fragment_information.coin_value = coin_value;
            receiver_fragment_information.coin_value = coin_value;
            break;
        }
        case AMP22_MST_ROUND_PHASE_ACTION_TYPE_INTER_FRAGMENT_COMMUNICATION_2_1: {
            int fragment_id = AMP22_MST_message->getFragmentId();
            int parent = AMP22_MST_message->getParent();
            int level = AMP22_MST_message->getLevel();
            sender_fragment_information.fragment_id = fragment_id;
            sender_fragment_information.parent = parent;
            sender_fragment_information.level = level;
            break;
        }
        default:
            break;
        }

        AMP22MSTMessage *new_msg = AMP22_MST_message->dup();
        new_msg->setSenderId(id);
        new_msg->setReceiverId(receiver_id);
        if (AMP22_MST_message->getIsMSTEdge() &&
            AMP22_MST_message->getCoinValue() == AMP22_MST_COIN_VALUE_TAIL) {
            CDS_status = IN_CDS;
        }
        send_new_message(new_msg);
        processed_msg.push_back(AMP22_MST_message);
    }

    for (AMP22MSTMessage *msg : processed_msg) {
        message_set.erase(msg);
        delete msg;
    }
}

void ThreeHopAMP22MSTAlg::listen_new_message(cMessage *msg)
{
    if (MIS_status == IN_MIS) return IAlgNode::listen_new_message(msg);

    EV << "is_awake() = " << is_awake() << '\n';
    if (!is_awake())
        delete msg;
    else {
        AMP22MSTMessage *AMP22_MST_message =
            dynamic_cast<AMP22MSTMessage *>(msg);
        message_set.insert(AMP22_MST_message);
        last_communication_round = current_round_id;
        EV << "is_awake() = " << is_awake() << ' '
           << "message_set.size() = " << message_set.size() << '\n';
    }
}

void ThreeHopAMP22MSTAlg::clear_message_queue()
{
    if (MIS_status == IN_MIS) return IAlgNode::clear_message_queue();
    for (cMessage *msg : message_set) delete msg;
    message_set.clear();
}

bool ThreeHopAMP22MSTAlg::is_awake()
{
    if (MIS_status == IN_MIS) return AMP22MSTAlg::is_awake();
    return awake_rounds_in_phase_set.count(round_id_in_current_phase) > 0;
}

ThreeHopAMP22MSTAlg::~ThreeHopAMP22MSTAlg() { clear_message_queue(); }