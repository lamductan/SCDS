#include <algorithm>
#include <iomanip>

#include "algorithms/algs.h"
#include "algorithms/ialg_node.h"
#include "algorithms/network_alg.h"
#include "checkers/checker_factory.h"
#include "common.h"
#include "messages/messages.h"
#include "networks/Network.h"
#include "node/Node.h"
#include "utils/utils.h"

void Network::initialize()
{
    network_file = par("networkFile").stringValue();
    alg_name = par("algorithm").stringValue();
    setDisplayString("p=0,0");
    scheduleAt(
        0, new cMessage("Initialize the network!", SELF_INITIALIZE_MESSAGE));
}

void Network::handleMessage(cMessage *msg)
{
    EV << msg << '\n';
    if (msg->isSelfMessage() && msg->getKind() == SELF_INITIALIZE_MESSAGE) {
        buildNetwork(getParentModule());
        scheduleAt(1, new cMessage("Start Rounds", START_ROUND_MESSAGE));
        delete msg;
    } else {
        alg->handle_message(msg);
    }
}

/**
 * @brief
 * Build Network from file
 * @param parent
 */
void Network::buildNetwork(cModule *parent)
{
    EV << "buildNetwork " << parent->getName() << '\n';
    std::string line;
    char modtypename[] = "node.Node";
    cModuleType *modtype = cModuleType::find(modtypename);
    if (!modtype)
        throw cRuntimeError("module type `%s' not found", modtypename);

    int n, m;
    std::fstream networkFile(network_file, std::ios::in);
    while (getline(networkFile, line, '\n')) {
        if (line.empty() || line[0] == '#') continue;

        std::vector<std::string> tokens =
            cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 4)
            throw cRuntimeError(
                "wrong line in module file: 4 items required, line: \"%s\"",
                line.c_str());

        n = atoi(tokens[0].c_str());
        m = atoi(tokens[1].c_str());
        break;
    }

    n_nodes = n;

    for (int i = 0; i < n; ++i) {
        // get fields from tokens
        getline(networkFile, line, '\n');
        std::vector<std::string> tokens =
            cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 3)
            throw cRuntimeError("wrong line when reading node content: 3 items "
                                "required, line: \"%s\"",
                                line.c_str());
        int nodeid = atoi(tokens[0].c_str());
        int x = atoi(tokens[1].c_str());
        int y = atoi(tokens[2].c_str());
        EV << "NODE id=" << nodeid << ", x=" << x << ", y=" << y << "\n";

        // create module
        char name[10] = "node";
        strcat(name, tokens[0].c_str());
        cModule *mod = modtype->create(name, parent);
        cPar &parId = mod->par("id");
        parId.setIntValue(nodeid);
        cPar &parX = mod->par("x");
        parX.setIntValue(x);
        cPar &parY = mod->par("y");
        parY.setIntValue(y);
        cPar &parNNodes = mod->par("n_nodes");
        parNNodes.setIntValue(n);

        Node *node_ptr = dynamic_cast<Node *>(mod);
        nodes[nodeid] = node_ptr;

        // read params from the ini file, etc
        mod->finalizeParameters();
        std::string displayString =
            "p=" + std::to_string(x) + "," + std::to_string(y);
        mod->setDisplayString(displayString.c_str());
        nodesWirelessIn[node_ptr] = mod->gate("wirelessIn");
    }

    // read and create connections
    for (int i = 0; i < m; ++i) {
        getline(networkFile, line, '\n');
        std::vector<std::string> tokens =
            cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 2)
            throw cRuntimeError("wrong line when reading edge content: 2 items "
                                "required, line: \"%s\"",
                                line.c_str());

        // get fields from tokens
        int srcnodeid = atoi(tokens[0].c_str());
        int destnodeid = atoi(tokens[1].c_str());
        // EV << "Edge " << srcnodeid << ' ' << destnodeid << '\n';

        if (nodes.find(srcnodeid) == nodes.end())
            throw cRuntimeError(
                "wrong line in connections file: node with id=%d not found",
                srcnodeid);
        if (nodes.find(destnodeid) == nodes.end())
            throw cRuntimeError(
                "wrong line in connections file: node with id=%d not found",
                destnodeid);

        Node *srcnode = nodes[srcnodeid];
        Node *destnode = nodes[destnodeid];

        cGate *srcIn, *srcOut, *destIn, *destOut;
        srcnode->getOrCreateFirstUnconnectedGatePair("port", false, true, srcIn,
                                                     srcOut);
        destnode->getOrCreateFirstUnconnectedGatePair("port", false, true,
                                                      destIn, destOut);

        // connect
        srcnode->connect(srcOut, destIn, destnodeid);
        destnode->connect(destOut, srcIn, srcnodeid);
    }

    // initialization will simply skip already-initialized modules instead of
    // causing error
    EV << "Done buildNetwork " << parent->getName() << '\n';
    networkFile.close();
    alg = new NetworkAlg(this, alg_name);
    for (auto it : nodes) {
        Node *node = it.second;
        node->network_alg = alg;
    }
    centralized_graph = construct_graph();
    centralized_graph->find_three_hop_neighbors();
    parent->callInitialize();
}

std::vector<int> Network::get_selected_nodes()
{
    std::vector<int> selected_nodes;
    for (auto it : nodes) {
        Node *node = it.second;
        if (node->alg->is_selected()) selected_nodes.push_back(node->id);
    }
    std::sort(selected_nodes.begin(), selected_nodes.end());
    return selected_nodes;
}

int Network::get_total_awake_rounds()
{
    int total_awake_rounds = 0;
    for (auto it : nodes) {
        Node *node = it.second;
        total_awake_rounds += node->alg->n_awake_rounds;
    }
    return total_awake_rounds;
}

int Network::get_max_awake_rounds()
{
    int max_awake_rounds = 0;
    for (auto it : nodes) {
        Node *node = it.second;
        max_awake_rounds =
            std::max(max_awake_rounds, node->alg->n_awake_rounds);
    }
    return max_awake_rounds;
}

int Network::get_finished_round()
{
    int finished_round = -1;
    for (auto it : nodes) {
        Node *node = it.second;
        finished_round = std::max(finished_round,
                                  std::max(node->alg->last_communication_round,
                                           node->alg->decided_round));
    }
    return finished_round;
}

centralized::Graph *Network::construct_graph()
{
    for (auto it : nodes) {
        int nodeid = it.first;
        Node *node = it.second;
        centralized::Node *centralized_node =
            new centralized::Node(0, 0, nodeid);
        centralized_nodes_map[nodeid] = centralized_node;
    }
    std::set<centralized::Edge> edges_from_original_graph;
    for (auto it : centralized_nodes_map) {
        int nodeid = it.first;
        centralized::Node *centralized_node = it.second;
        Node *node = nodes[nodeid];
        for (int neighbor_id : node->all_neighbors) {
            if (neighbor_id < nodeid) continue;
            edges_from_original_graph.insert({ nodeid, neighbor_id });
        }
    }
    centralized::Graph *centralized_graph =
        new centralized::Graph(centralized_nodes_map, edges_from_original_graph,
                               centralized::CDS_SIMPLE_NODE_TYPE);
    return centralized_graph;
}

bool Network::check(bool is_final_check)
{
    IChecker *checker = CheckerFactory::create_checker(this);
    bool result = checker->check(is_final_check);
    delete checker;
    return result;
}

void Network::log_result()
{
    int total_awake_rounds = get_total_awake_rounds();
    int max_awake_rounds = get_max_awake_rounds();
    int finished_round = get_finished_round();
    std::vector<int> selected_nodes = get_selected_nodes();

    std::vector<std::string> tokens =
        cStringTokenizer(network_file, "/").asVector();
    EV << "network_file = " << network_file << '\n';
    tokens[0] = "results";
    std::string basename = tokens.back();
    tokens.pop_back();
    std::string log_basename = std::string(alg_name) + "_" + basename;
    std::string log_dir = join_path(tokens);
    std::string log_path = log_dir + "/" + log_basename;

    EV << "log_path = " << log_path << '\n';
    mkPath(log_dir.c_str());

    std::fstream f(log_path.c_str(), std::ios::out);

    std::vector<int> node_ids = getMapKeys<int, Node *>(nodes);
    std::vector<std::vector<bool>> all_awake_round_vec(n_nodes);
    EV << "finished_round = " << finished_round << '\n';
    for (int node_id : node_ids) {
        Node *node = nodes[node_id];
        std::vector<bool> awake_round_vec =
            map_to_vector(node->alg->awake_round_map, finished_round);
        all_awake_round_vec[node_id] = awake_round_vec;
    }

    std::vector<int> idle_rounds;
    for (int i = 1; i < finished_round; ++i) {
        if (is_idle_round(all_awake_round_vec, i)) idle_rounds.push_back(i);
    }

    f << "#n n_selected_nodes total_awake_rounds average_awake_rounds "
         "max_awake_rounds finished_round n_idle_rounds n_active_rounds\n";
    f << n_nodes << ' ' << selected_nodes.size() << ' ' << total_awake_rounds
      << ' ' << 1.0 * total_awake_rounds / n_nodes << ' ' << max_awake_rounds
      << ' ' << finished_round << ' ' << idle_rounds.size() << ' '
      << (finished_round - idle_rounds.size()) << "\n";

    IChecker *checker = CheckerFactory::create_checker(this);
    bool check_result = checker->check();
    f << "Check result = " << check_result << '\n';

    for (int node_id : node_ids) {
        Node *node = nodes[node_id];
        f << std::setfill('0') << std::setw(4) << node_id << ' '
          << node->alg->n_awake_rounds << ' ' << node->alg->decided_round << ' '
          << node->alg->last_communication_round << '\n';
    }

    /*
    f << "#idle rounds:\n";
    for (int x : idle_rounds) f << x << ' ';
    f << '\n';

    f << "\n#node_id node_n_awake_rounds node_decided_round "
         "node_last_communication_round\n";
    for (int node_id : node_ids) {
        Node *node = nodes[node_id];
        f << std::setfill('0') << std::setw(4) << node_id << ' '
          << node->alg->n_awake_rounds << ' ' << node->alg->decided_round << ' '
          << node->alg->last_communication_round << '\n';
    }

    f << "\n#awake_rounds\n";
    for (int node_id : node_ids) {
        f << std::setfill('0') << std::setw(4) << node_id << ' ';
        for (int i = 1; i < all_awake_round_vec[node_id].size(); ++i)
            f << all_awake_round_vec[node_id][i] << ' ';
        f << '\n';
    }
    */

    f << "\n#selected nodes:\n";
    for (int selected_node : selected_nodes) f << selected_node << '\n';
    f << "Check result = " << check_result << '\n';
    delete checker;
    f.close();
}

void Network::finish()
{
    log_result();
    IChecker *checker = CheckerFactory::create_checker(this);
    EV << "Check result = " << checker->check();
    delete checker;
}

Network::~Network()
{
    delete alg;
    delete centralized_graph;
    for (auto it : centralized_nodes_map) delete it.second;
}