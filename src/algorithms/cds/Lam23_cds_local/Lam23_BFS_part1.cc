#include "algorithms/cds/Lam23_cds_local/Lam23_BFS_part1.h"
#include "utils/utils.h"
#include <cassert>

void LamBFSPart1Alg::set_alg_type()
{
    EV << "LamBFSPart1Alg::set_alg_type()\n";
    alg_type = DUMMY_ALG;
}

LamBFSPart1Alg::LamBFSPart1Alg() {}

void LamBFSPart1Alg::init(Node *node, int starting_round)
{
    IAlgNode::init(node, starting_round);
    fragment_information.fragment_id = id;
    fragment_information.fragment_status = true;
    fragment_information.fragment_diameter = 0;
    fragment_information.fragment_nodes = { id };
    bfs_tree_structure = { id, 0, -1, -1 };

    different_fragment_neighbors = neighbors_set;

    fragment_information.fragment_bfs_tree_structure =
        std::map<int, centralized::BFSTreeStructure>();

    max_num_rounds = 3; // TODO: may change for larger diameter
    d = 1;              // TODO: may change for larger diameter

    for (int neighbor_id : node->all_neighbors) {
        centralized::Edge edge(id, neighbor_id);
        std::tuple<int, int, int> edge_id = edge.get_id();
        basic_edges_set.insert(edge_id);
        fragment_information.edges_adj_fragment_nodes.insert(edge_id);
        edges_map[neighbor_id] = edge_id;
    }

    node_moe = fragment_moe =
        (basic_edges_set.empty()) ? INF_EDGE_ID : *basic_edges_set.begin();
    new_fragment_information = fragment_information;
}

LamBFSPart1Alg::LamBFSPart1Alg(Node *node, int starting_round)
{
    init(node, starting_round);
}

cMessage *LamBFSPart1Alg::process_message_queue()
{
    EV << "LamBFSPart1Alg::process_message_queue -- message_queue.size() = "
       << message_queue.size() << '\n';

    cMessage *new_msg = nullptr;
    switch (current_round_id) {
    case 1:
        new_msg = first_round_send_coin_value_message();
    case 2:
        new_msg = second_round_send_bfs_structure_message();
    case 3:
        new_msg = third_round_receive_bfs_structure_message();
    default:
        new_msg = nullptr;
    }
    print_state();
    return new_msg;
}

cMessage *LamBFSPart1Alg::first_round_send_coin_value_message()
{
    if (basic_edges_set.empty()) return nullptr;
    coin_value = toss_coin();
    if (coin_value) return nullptr;
    LamBFSMessage *Lam_bfs_message = create_Lam_bfs_message();
    int receiver_id = get_neighbor_id_of_edge(Lam_bfs_message->getMOE(), id);
    need_to_send = { receiver_id };
    return Lam_bfs_message;
}

cMessage *LamBFSPart1Alg::second_round_send_bfs_structure_message()
{
    if (coin_value == false) return nullptr;
    need_to_send.clear();
    for (cMessage *msg : message_queue) {
        LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
        int sender_id = Lam_bfs_message->getSenderId();
        bool sender_coin_value = Lam_bfs_message->getCoinValue();
        assert(sender_coin_value == false);
        update_new_fragment_information_from_msg(Lam_bfs_message);
        need_to_send.insert(sender_id);
    }

    fragment_root_compute_new_bfs_tree_structure();
    LamBFSMessage *Lam_bfs_message = create_Lam_bfs_message();
    return Lam_bfs_message;
}

cMessage *LamBFSPart1Alg::third_round_receive_bfs_structure_message()
{
    if (message_queue.size() == 1) {
        LamBFSMessage *Lam_bfs_message =
            dynamic_cast<LamBFSMessage *>(message_queue[0]);
        new_fragment_information = Lam_bfs_message->getFragmentInformation();
    }
    update_fragment_information();
    return nullptr;
}

void LamBFSPart1Alg::update_fragment_information()
{
    if (updated_fragment_information_in_current_phase) return;

    EV << "LamBFSPart1Alg::update_fragment_information()\n";
    fragment_information = new_fragment_information;

    different_fragment_neighbors = diff_set<int>(
        different_fragment_neighbors, fragment_information.fragment_nodes);

    basic_edges_set.clear();
    for (int different_fragment_neighbor : different_fragment_neighbors) {
        basic_edges_set.insert(edges_map[different_fragment_neighbor]);
    }

    bfs_tree_structure = fragment_information.fragment_bfs_tree_structure[id];
    updated_fragment_information_in_current_phase = true;
}

void LamBFSPart1Alg::update_new_fragment_information_from_msg(
    LamBFSMessage *Lam_bfs_message)
{
    LamBFSFragmentInformation received_new_fragment_information =
        Lam_bfs_message->getNewFragmentInformation();
    new_fragment_information.fragment_nodes =
        combine_set<int>(new_fragment_information.fragment_nodes,
                         received_new_fragment_information.fragment_nodes);
    new_fragment_information.edges_adj_fragment_nodes =
        combine_set<std::tuple<int, int, int>>(
            new_fragment_information.edges_adj_fragment_nodes,
            received_new_fragment_information.edges_adj_fragment_nodes);
}

bool LamBFSPart1Alg::fragment_root_compute_new_bfs_tree_structure()
{
    centralized::Graph *new_graph = new centralized::Graph(
        new_fragment_information.fragment_nodes,
        new_fragment_information.edges_adj_fragment_nodes);
    new_fragment_information.fragment_bfs_tree_structure =
        new_graph->find_bfs_tree_structure_from_root(id);
    new_fragment_information.fragment_diameter =
        centralized::Graph::get_max_level(
            new_fragment_information.fragment_bfs_tree_structure);
    new_fragment_information.fragment_id = id;
    new_fragment_information.fragment_status =
        (new_fragment_information.fragment_diameter <= d);
    return new_fragment_information.fragment_status;
}

bool LamBFSPart1Alg::is_awake() { return true; }

bool LamBFSPart1Alg::is_decided()
{
    return fragment_information.fragment_diameter == 1;
}

bool LamBFSPart1Alg::is_selected() { return true; }

LamBFSMessage *LamBFSPart1Alg::create_Lam_bfs_message()
{
    LamBFSMessage *Lam_bfs_message = new LamBFSMessage();
    Lam_bfs_message->setSenderId(id);
    Lam_bfs_message->setSentRoundId(current_round_id);
    Lam_bfs_message->setFragmentInformation(fragment_information);
    Lam_bfs_message->setNewFragmentInformation(new_fragment_information);
    Lam_bfs_message->setCoinValue(coin_value);
    Lam_bfs_message->setMOE(fragment_moe);
    return Lam_bfs_message;
}

LamBFSCoinValue LamBFSPart1Alg::toss_coin() const
{
    return (node->bernoulli(0.5)) ? LAM_BFS_COIN_VALUE_HEAD
                                  : LAM_BFS_COIN_VALUE_TAIL;
}

void LamBFSPart1Alg::print_state(int log_level)
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "state = {\n";
    s += (tab + one_tab +
          "id                          = " + std::to_string(id) + ",\n");
    s += (tab + one_tab + "fragment_information        = " +
          fragment_information.to_string(log_level + 1, false) + ",\n");
    s += (tab + one_tab + "new_fragment_information    = " +
          new_fragment_information.to_string(log_level + 1, false) + ",\n");
    s += (tab + one_tab + "different_fragment_neighbors= " +
          set_to_string<int>(different_fragment_neighbors) + ",\n");
    s += (tab + one_tab + "basic_edges_set             = " +
          set_tuple_to_string(basic_edges_set, log_level + 1) + "\n");
    s += (tab + one_tab + "coin_value                  = " +
          LamBFSCoinValue_to_string(coin_value) + ",\n");
    s += (tab + one_tab + "bfs_tree_structure          = " +
          bfs_tree_structure.to_string(log_level + 1, false) + ",\n");
    s += (tab + one_tab +
          "node_moe                    = " + tuple_to_string(node_moe) + ",\n");
    s += tab + "}";
    EV << s << '\n';
}

void LamBFSPart1Alg::print_message_queue()
{
    EV << "LamBFSPart1Alg::print_message_queue()\n";
    for (cMessage *msg : message_queue) {
        LamBFSMessage *Lam_bfs_message = dynamic_cast<LamBFSMessage *>(msg);
        EV << Lam_bfs_message->to_string(1) << '\n';
    }
}