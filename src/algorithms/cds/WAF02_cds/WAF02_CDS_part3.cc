#include "algorithms/cds/WAF02_cds/WAF02_CDS_part3.h"
#include "algorithms/cds/WAF02_cds/WAF02_TwoHopMISPart2.h"
#include "utils/utils.h"

void WAF02CDSPart3Alg::set_alg_type()
{
    EV << "WAF02CDSPart3Alg::set_alg_type()\n";
    alg_type = CDS_ALG;
}

WAF02CDSPart3Alg::WAF02CDSPart3Alg() {}

void WAF02CDSPart3Alg::init_from_two_hop_MIS_alg(
    WAF02TwoHopMISAlg *WAF02_two_hop_MIS_alg)
{
    WAF02TwoHopMISPart2Alg *WAF02_two_hop_MIS_part2_alg =
        WAF02_two_hop_MIS_alg->WAF02_TwoHopMIS_part2_alg;
    starting_round = WAF02_two_hop_MIS_part2_alg->decided_round + 1;
    IAlgNode::init(WAF02_two_hop_MIS_part2_alg->node, starting_round);
    x1 = node->all_neighbors.size();
    mis_neighbors = WAF02_two_hop_MIS_part2_alg->mis_neighbors;
    waiting_message_from = neighbors_set;
    is_MIS_root = (WAF02_two_hop_MIS_part2_alg->level == 0);
    MIS_status = WAF02_two_hop_MIS_part2_alg->MIS_status;
}

bool WAF02CDSPart3Alg::is_awake()
{
    awake_round_map[current_round_id] = true;
    return true;
}

bool WAF02CDSPart3Alg::is_selected() { return CDS_status == IN_CDS; }

bool WAF02CDSPart3Alg::is_decided()
{
    return CDS_status == IN_CDS || waiting_message_from.empty();
}

void WAF02CDSPart3Alg::process_round()
{
    EV << "WAF02CDSPart3Alg::process_round()\n";
    EV << "n_awake_rounds = " << n_awake_rounds << '\n';
    if (!is_awake()) {
        EV << "not awake\n";
        return;
    }
    ++n_awake_rounds;
    EV << "Is awake: n_awake_rounds = " << n_awake_rounds << '\n';
    EV << "pre_process: message_queue.size() = " << message_queue.size()
       << '\n';
    process_WAF02_CDS_message_queue();
    if (current_round_id == starting_round && is_MIS_root) {
        MIS_root_send_first_QUERY_message();
    }
    // print_state();
    decide_CDS_status();
    clear_message_queue();
}

void WAF02CDSPart3Alg::MIS_root_send_first_QUERY_message()
{
    WAF02CDSMessage *query_msg = create_WAF02_CDS_message();
    query_msg->setWAF02CDSMessageType(WAF02_CDS_MESSAGE_QUERY);
    broadcast_message(query_msg);
}

void WAF02CDSPart3Alg::process_WAF02_CDS_message_queue()
{
    EV << "\t"
       << "WAF02CDSPart3Alg::process_WAF02_CDS_message_queue()\n";
    for (cMessage *msg : message_queue) {
        WAF02CDSMessage *WAF02_CDS_message =
            dynamic_cast<WAF02CDSMessage *>(msg);
        if (WAF02_CDS_message == nullptr) continue;
        // EV << WAF02_CDS_message->to_string(1) << '\n';
        switch (WAF02_CDS_message->getWAF02CDSMessageType()) {
        case WAF02_CDS_MESSAGE_QUERY:
            process_QUERY_message(WAF02_CDS_message);
            break;
        case WAF02_CDS_MESSAGE_REPORT:
            process_REPORT_message(WAF02_CDS_message);
            break;
        case WAF02_CDS_MESSAGE_ROOT:
            process_ROOT_message(WAF02_CDS_message);
            break;
        case WAF02_CDS_MESSAGE_INVITE1:
            process_INVITE1_message(WAF02_CDS_message);
            break;
        case WAF02_CDS_MESSAGE_INVITE2:
            process_INVITE2_message(WAF02_CDS_message);
            break;
        case WAF02_CDS_MESSAGE_JOIN:
            process_JOIN_message(WAF02_CDS_message);
            break;
        default:
            break;
        }
    }
}

void WAF02CDSPart3Alg::process_QUERY_message(WAF02CDSMessage *WAF02_CDS_message)
{
    int sender_id = WAF02_CDS_message->getSenderId();
    WAF02CDSMessage *report_msg = create_WAF02_CDS_message();
    report_msg->setWAF02CDSMessageType(WAF02_CDS_MESSAGE_REPORT);
    send_message_to(report_msg, sender_id);
}

void WAF02CDSPart3Alg::process_REPORT_message(
    WAF02CDSMessage *WAF02_CDS_message)
{
    int sender_id = WAF02_CDS_message->getSenderId();
    --x1;
    int sender_n_mis_neighbors = WAF02_CDS_message->getNMISNeighbors();
    if (sender_n_mis_neighbors > degree) {
        degree = sender_n_mis_neighbors;
        root = sender_id;
    }

    if (x1 > 0) return;
    WAF02CDSMessage *root_msg = create_WAF02_CDS_message();
    root_msg->setWAF02CDSMessageType(WAF02_CDS_MESSAGE_ROOT);
    send_message_to(root_msg, root);
}

void WAF02CDSPart3Alg::process_ROOT_message(WAF02CDSMessage *WAF02_CDS_message)
{
    is_CDS_root = true;
    z = 1;
    CDS_status = IN_CDS;
    WAF02CDSMessage *invite2_msg = create_WAF02_CDS_message();
    invite2_msg->setWAF02CDSMessageType(WAF02_CDS_MESSAGE_INVITE2);
    broadcast_message(invite2_msg);
}

void WAF02CDSPart3Alg::process_INVITE1_message(
    WAF02CDSMessage *WAF02_CDS_message)
{
    int sender_id = WAF02_CDS_message->getSenderId();
    waiting_message_from.erase(sender_id);
    if (MIS_status != NOT_IN_MIS) return;
    if (z == 1) return;
    z = 1;
    parent_id = sender_id;

    WAF02CDSMessage *join_msg = create_WAF02_CDS_message();
    join_msg->setWAF02CDSMessageType(WAF02_CDS_MESSAGE_JOIN);
    send_message_to(join_msg, sender_id);

    WAF02CDSMessage *invite2_msg = create_WAF02_CDS_message();
    invite2_msg->setWAF02CDSMessageType(WAF02_CDS_MESSAGE_INVITE2);
    broadcast_message(invite2_msg);
}

void WAF02CDSPart3Alg::process_INVITE2_message(
    WAF02CDSMessage *WAF02_CDS_message)
{
    int sender_id = WAF02_CDS_message->getSenderId();
    waiting_message_from.erase(sender_id);
    if (MIS_status != IN_MIS) return;
    if (z == 1) return;
    z = 1;
    parent_id = sender_id;

    WAF02CDSMessage *join_msg = create_WAF02_CDS_message();
    join_msg->setWAF02CDSMessageType(WAF02_CDS_MESSAGE_JOIN);
    send_message_to(join_msg, sender_id);

    WAF02CDSMessage *invite1_msg = create_WAF02_CDS_message();
    invite1_msg->setWAF02CDSMessageType(WAF02_CDS_MESSAGE_INVITE1);
    broadcast_message(invite1_msg);
}

void WAF02CDSPart3Alg::process_JOIN_message(WAF02CDSMessage *WAF02_CDS_message)
{
    int sender_id = WAF02_CDS_message->getSenderId();
    waiting_message_from.erase(sender_id);
    children.insert(sender_id);
}

WAF02CDSMessage *WAF02CDSPart3Alg::create_WAF02_CDS_message()
{
    WAF02CDSMessage *msg = new WAF02CDSMessage();
    msg->setSenderId(id);
    msg->setNMISNeighbors(mis_neighbors.size());
    return msg;
}

void WAF02CDSPart3Alg::broadcast_message(WAF02CDSMessage *WAF02_CDS_message)
{
    // EV << "WAF02CDSPart3Alg::broadcast_message()\n";
    for (int neighbor_id : node->all_neighbors) {
        node->sendDelayed(WAF02_CDS_message->dup(), DELAY_SENT,
                          node->neighbor_gates[neighbor_id]);
    }
    // EV << WAF02_CDS_message->to_string() << '\n';
    // EV << "broadcast = " << vector_to_string(node->all_neighbors);
    // EV << "\n\n";
    delete WAF02_CDS_message;
}

void WAF02CDSPart3Alg::send_message_to(WAF02CDSMessage *WAF02_CDS_message,
                                       int receiver_id)
{
    node->sendDelayed(WAF02_CDS_message, DELAY_SENT,
                      node->neighbor_gates[receiver_id]);
    // EV << WAF02_CDS_message->to_string() << '\n';
    // EV << "receiver_id = " << receiver_id;
    // EV << "\n\n";
}

void WAF02CDSPart3Alg::decide_CDS_status()
{
    if (CDS_status == IN_CDS) return;
    if (z == 1 && !children.empty()) CDS_status = IN_CDS;
}

void WAF02CDSPart3Alg::print_state(int log_level)
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "state = {\n";
    s += (tab + one_tab + "id                   = " + std::to_string(id) +
          ",\n");
    s += (tab + one_tab +
          "parent_id            = " + std::to_string(parent_id) + ",\n");
    s += (tab + one_tab + "root                 = " + std::to_string(root) +
          ",\n");
    s += (tab + one_tab + "degree               = " + std::to_string(degree) +
          ",\n");
    s += (tab + one_tab + "x1                   = " + std::to_string(x1) +
          ",\n");
    s +=
        (tab + one_tab + "z                    = " + std::to_string(z) + ",\n");
    s += (tab + one_tab +
          "is_CDS_root          = " + std::to_string(is_CDS_root) + ",\n");
    s += (tab + one_tab + "mis_neighbors        = " +
          set_to_string<int>(mis_neighbors) + ",\n");
    s += (tab + one_tab +
          "children             = " + set_to_string<int>(children) + ",\n");
    s += (tab + one_tab + "waiting_message_from = " +
          set_to_string<int>(waiting_message_from) + ",\n");
    s += (tab + one_tab +
          "CDS_status           = " + CDS_status_to_string(CDS_status) + ",\n");
    s += (tab + one_tab +
          "is_decided()         = " + std::to_string(is_decided()) + ",\n");
    s += (tab + "}");
    EV << s << '\n';
}