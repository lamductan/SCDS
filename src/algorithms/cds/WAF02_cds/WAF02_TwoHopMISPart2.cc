#include "algorithms/cds/WAF02_cds/WAF02_TwoHopMISPart2.h"
#include "utils/utils.h"

void WAF02TwoHopMISPart2Alg::set_alg_type()
{
    EV << "WAF02TwoHopMISPart2Alg::set_alg_type()\n";
    alg_type = TWO_HOP_MIS_ALG;
}

WAF02TwoHopMISPart2Alg::WAF02TwoHopMISPart2Alg() {}

void WAF02TwoHopMISPart2Alg::init_from_bfs_alg(BFSAlg *bfs_alg)
{
    starting_round = bfs_alg->decided_round + 1;
    IAlgNode::init(bfs_alg->node, starting_round);
    parent_id = bfs_alg->parent_id;
    level = bfs_alg->level;
    rank = { level, id };
    children = bfs_alg->children;
    x2 = children.size();
    lower_rank_neighbors = get_lower_rank_neighbors(bfs_alg->neighbors_rank);
    y = lower_rank_neighbors.size();
}

std::set<std::pair<int, int>> WAF02TwoHopMISPart2Alg::get_lower_rank_neighbors(
    const std::set<std::pair<int, int>> &neighbors_rank) const
{
    std::set<std::pair<int, int>> lower_rank_neighbors;
    for (const std::pair<int, int> &neighbor_rank : neighbors_rank) {
        if (neighbor_rank < rank) lower_rank_neighbors.insert(neighbor_rank);
    }
    return lower_rank_neighbors;
}

bool WAF02TwoHopMISPart2Alg::is_awake()
{
    awake_round_map[current_round_id] = true;
    return true;
}

bool WAF02TwoHopMISPart2Alg::is_selected() { return MIS_status == IN_MIS; }

bool WAF02TwoHopMISPart2Alg::is_decided()
{
    return MIS_status != UNDECIDED && x2 == 0;
}

void WAF02TwoHopMISPart2Alg::process_round()
{
    EV << "WAF02TwoHopMISPart2Alg::process_round()\n";
    EV << "n_awake_rounds = " << n_awake_rounds << '\n';
    if (!is_awake()) {
        EV << "not awake\n";
        return;
    }
    ++n_awake_rounds;
    EV << "Is awake: n_awake_rounds = " << n_awake_rounds << '\n';
    EV << "pre_process: message_queue.size() = " << message_queue.size()
       << '\n';
    process_WAF02_two_hop_MIS_message_queue();
    if (current_round_id == starting_round && level == 0) {
        MIS_status = IN_MIS;
        root_send_first_black_message();
    }
    // print_state();
    if (level > 0) send_complete_message_to_parent_when_avaiable();
    clear_message_queue();
}

void WAF02TwoHopMISPart2Alg::root_send_first_black_message()
{
    WAF02TwoHopMISMessage *black_msg = create_WAF02_two_hop_MIS_message();
    black_msg->setWAF02TwoHopMISMessageType(WAF02_TWO_HOP_MIS_MESSAGE_BLACK);
    broadcast_message(black_msg);
    sent_black_message = true;
}

void WAF02TwoHopMISPart2Alg::process_WAF02_two_hop_MIS_message_queue()
{
    EV << "\t"
       << "WAF02TwoHopMISPart2Alg::process_WAF02_two_hop_MIS_message_queue()\n";
    for (cMessage *msg : message_queue) {
        WAF02TwoHopMISMessage *WAF02_two_hop_MIS_message =
            dynamic_cast<WAF02TwoHopMISMessage *>(msg);
        if (WAF02_two_hop_MIS_message == nullptr) continue;
        // EV << WAF02_two_hop_MIS_message->to_string(1) << '\n';
        switch (WAF02_two_hop_MIS_message->getWAF02TwoHopMISMessageType()) {
        case WAF02_TWO_HOP_MIS_MESSAGE_BLACK:
            process_black_message(WAF02_two_hop_MIS_message);
            break;
        case WAF02_TWO_HOP_MIS_MESSAGE_GRAY:
            process_gray_message(WAF02_two_hop_MIS_message);
            break;
        case WAF02_TWO_HOP_MIS_MESSAGE_MARK_COMPLETE:
            process_mark_complete_message(WAF02_two_hop_MIS_message);
            break;
        default:
            break;
        }
    }
}

void WAF02TwoHopMISPart2Alg::process_black_message(
    WAF02TwoHopMISMessage *WAF02_two_hop_MIS_message)
{
    int sender_id = WAF02_two_hop_MIS_message->getSenderId();
    int received_level = WAF02_two_hop_MIS_message->getLevel();
    mis_neighbors.insert(sender_id);
    if (MIS_status == UNDECIDED) {
        MIS_status = NOT_IN_MIS;
        if (sent_gray_message) return;
        WAF02TwoHopMISMessage *gray_msg = create_WAF02_two_hop_MIS_message();
        gray_msg->setWAF02TwoHopMISMessageType(WAF02_TWO_HOP_MIS_MESSAGE_GRAY);
        broadcast_message(gray_msg);
        sent_gray_message = true;
    }
}

void WAF02TwoHopMISPart2Alg::process_gray_message(
    WAF02TwoHopMISMessage *WAF02_two_hop_MIS_message)
{
    int sender_id = WAF02_two_hop_MIS_message->getSenderId();
    int received_level = WAF02_two_hop_MIS_message->getLevel();
    if (std::pair<int, int>(received_level, sender_id) < rank &&
        MIS_status == UNDECIDED) {
        --y;
    }
    if (y == 0 && MIS_status == UNDECIDED) {
        MIS_status = IN_MIS;
        if (sent_black_message) return;
        WAF02TwoHopMISMessage *black_msg = create_WAF02_two_hop_MIS_message();
        black_msg->setWAF02TwoHopMISMessageType(
            WAF02_TWO_HOP_MIS_MESSAGE_BLACK);
        broadcast_message(black_msg);
        sent_black_message = true;
    }
}

void WAF02TwoHopMISPart2Alg::process_mark_complete_message(
    WAF02TwoHopMISMessage *WAF02_two_hop_MIS_message)
{
    int sender_id = WAF02_two_hop_MIS_message->getSenderId();
    int received_level = WAF02_two_hop_MIS_message->getLevel();
    if (completed_children.count(sender_id) > 0) return;
    completed_children.insert(sender_id);
    --x2;
}

WAF02TwoHopMISMessage *
WAF02TwoHopMISPart2Alg::create_WAF02_two_hop_MIS_message()
{
    WAF02TwoHopMISMessage *msg = new WAF02TwoHopMISMessage();
    msg->setSenderId(id);
    msg->setLevel(level);
    return msg;
}

void WAF02TwoHopMISPart2Alg::broadcast_message(
    WAF02TwoHopMISMessage *WAF02_two_hop_MIS_message)
{
    // EV << "WAF02TwoHopMISPart2Alg::broadcast_message()\n";
    for (int neighbor_id : node->all_neighbors) {
        node->sendDelayed(WAF02_two_hop_MIS_message->dup(), DELAY_SENT,
                          node->neighbor_gates[neighbor_id]);
    }
    // EV << WAF02_two_hop_MIS_message->to_string() << '\n';
    // EV << "broadcast = " << vector_to_string(node->all_neighbors);
    // EV << "\n\n";
    delete WAF02_two_hop_MIS_message;
}

void WAF02TwoHopMISPart2Alg::send_complete_message_to_parent_when_avaiable()
{
    if (sent_mark_completed_message) return;
    if (children.size() > 0 && (MIS_status == UNDECIDED || x2 > 0)) return;
    if (children.size() == 0 && MIS_status == UNDECIDED) return;
    // EV << "WAF02TwoHopMISPart2Alg::send_complete_message_to_parent\n";
    sent_mark_completed_message = true;
    if (level == 0) return;
    WAF02TwoHopMISMessage *WAF02_two_hop_MIS_message =
        create_WAF02_two_hop_MIS_message();
    WAF02_two_hop_MIS_message->setWAF02TwoHopMISMessageType(
        WAF02_TWO_HOP_MIS_MESSAGE_MARK_COMPLETE);
    node->sendDelayed(WAF02_two_hop_MIS_message, DELAY_SENT,
                      node->neighbor_gates[parent_id]);
    // EV << WAF02_two_hop_MIS_message->to_string() << '\n';
    // EV << "need_to_send_node = " << parent_id;
    // EV << "\n\n";
}

void WAF02TwoHopMISPart2Alg::print_state(int log_level)
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "state = {\n";
    s += (tab + one_tab + "id                   = " + std::to_string(id) +
          ",\n");
    s += (tab + one_tab +
          "parent_id            = " + std::to_string(parent_id) + ",\n");
    s += (tab + one_tab +
          "children             = " + set_to_string<int>(children) + ",\n");
    s += (tab + one_tab + "lower_rank_neighbors = " +
          set_pair_to_string<int, int>(lower_rank_neighbors) + ",\n");
    s += (tab + one_tab + "level                = " + std::to_string(level) +
          ",\n");
    s += (tab + one_tab + "x2                   = " + std::to_string(x2) +
          ",\n");
    s +=
        (tab + one_tab + "y                    = " + std::to_string(y) + ",\n");
    s += (tab + one_tab + "mis_neighbors        = " +
          set_to_string<int>(mis_neighbors) + ",\n");
    s += (tab + one_tab + "completed_children   = " +
          set_to_string<int>(completed_children) + ",\n");
    s += (tab + one_tab +
          "MIS_status           = " + MIS_status_to_string(MIS_status) + ",\n");
    s += (tab + one_tab + "sent_black_message   = " +
          std::to_string(sent_black_message) + ",\n");
    s += (tab + one_tab + "sent_gray_message    = " +
          std::to_string(sent_gray_message) + ",\n");
    s += (tab + one_tab + "sent_mark_completed_message = " +
          std::to_string(sent_mark_completed_message) + ",\n");
    s += (tab + one_tab +
          "is_decided()         = " + std::to_string(is_decided()) + ",\n");
    s += (tab + "}");
    EV << s << '\n';
}