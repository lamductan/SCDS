#include <algorithm>

#include "common.h"
#include "utils/utils.h"
#include "networks/Network.h"
#include "node/Node.h"
#include "messages/messages.h"
#include "algorithms/network_alg.h"
#include "algorithms/ialg_node.h"
#include "checkers/checker_factory.h"

void Network::initialize()
{
    network_file = par("networkFile").stringValue();
    alg_name = par("algorithm").stringValue();
    setDisplayString("p=0,0");
    scheduleAt(0, new cMessage("Initialize the network!", SELF_INITIALIZE_MESSAGE));
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

void Network::connect(cGate *src, cGate *dest)
{
    src->connectTo(dest);
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
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 4)
            throw cRuntimeError("wrong line in module file: 4 items required, line: \"%s\"", line.c_str());

        n = atoi(tokens[0].c_str());
        m = atoi(tokens[1].c_str());
        break;
    }

    n_nodes = n;

    for(int i = 0; i < n; ++i) {
        // get fields from tokens
        getline(networkFile, line, '\n');
        std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 3)
            throw cRuntimeError("wrong line when reading node content: 3 items required, line: \"%s\"", line.c_str());
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
        nodes[nodeid] = dynamic_cast<Node *>(mod);

        // read params from the ini file, etc
        mod->finalizeParameters();
        std::string displayString = "p=" + std::to_string(x) + "," + std::to_string(y);
        mod->setDisplayString(displayString.c_str());
        nodesWirelessIn.push_back(mod->gate("wirelessIn"));
    }

    // read and create connections
    for(int i = 0; i < m; ++i) {
        getline(networkFile, line, '\n');
        std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 2)
            throw cRuntimeError("wrong line when reading edge content: 2 items required, line: \"%s\"", line.c_str());

        // get fields from tokens
        int srcnodeid = atoi(tokens[0].c_str());
        int destnodeid = atoi(tokens[1].c_str());
        //EV << "Edge " << srcnodeid << ' ' << destnodeid << '\n';

        if (nodes.find(srcnodeid) == nodes.end())
            throw cRuntimeError("wrong line in connections file: node with id=%d not found", srcnodeid);
        if (nodes.find(destnodeid) == nodes.end())
            throw cRuntimeError("wrong line in connections file: node with id=%d not found", destnodeid);

        cModule *srcmod = nodes[srcnodeid];
        cModule *destmod = nodes[destnodeid];

        cGate *srcIn, *srcOut, *destIn, *destOut;
        srcmod->getOrCreateFirstUnconnectedGatePair("port", false, true, srcIn, srcOut);
        destmod->getOrCreateFirstUnconnectedGatePair("port", false, true, destIn, destOut);

        // connect
        connect(srcOut, destIn);
        connect(destOut, srcIn);
    }

    // initialization will simply skip already-initialized modules instead of causing error
    EV << "Done buildNetwork " << parent->getName() << '\n';
    networkFile.close();
    alg = new NetworkAlg(this, alg_name);
    parent->callInitialize();
}

std::vector<int> Network::get_selected_nodes() {
    std::vector<int> selected_nodes;
    for(auto it : nodes) {
        Node *node = it.second;
        if (node->alg->is_selected()) selected_nodes.push_back(node->id);
    }
    std::sort(selected_nodes.begin(), selected_nodes.end());
    return selected_nodes;
}

int Network::get_total_awake_rounds() {
    int total_awake_rounds = 0;
    for(auto it : nodes) {
        Node *node = it.second;
        total_awake_rounds += node->alg->n_awake_rounds;
    }
    return total_awake_rounds;
}

int Network::get_finished_round() {
    int finished_round = -1;
    for(auto it : nodes) {
        Node *node = it.second;
        finished_round = std::max(finished_round, node->alg->last_communication_round);
    }
    return finished_round;
}

void Network::log_result() {
    int total_awake_rounds = get_total_awake_rounds();
    int finished_round = get_finished_round();
    std::vector<int> selected_nodes = get_selected_nodes();

    std::vector<std::string> tokens = cStringTokenizer(network_file, "/").asVector();
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
    f << n_nodes << ' ' << total_awake_rounds << ' ' << ceil(1.0*total_awake_rounds/n_nodes) 
      << ' ' << finished_round << ' ' << selected_nodes.size() << "\n";
    for(auto it : nodes) {
        Node *node = it.second;
        f << node->alg->n_awake_rounds << ' ' << node->alg->last_communication_round << '\n';
    }
    f << '\n';
    for(int selected_node : selected_nodes) f << selected_node << '\n';

    f.close();
}

void Network::finish() {
    log_result();
    IChecker *checker = CheckerFactory::create_checker(this);
    EV << "Check result = " << checker->check();
    delete checker;
}

Network::~Network() {
    delete alg;
}