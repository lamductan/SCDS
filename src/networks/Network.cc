#include "common.h"
#include "networks/Network.h"
#include "algorithms/alg_factory.h"
#include "messages/messages.h"

void Network::initialize()
{
    alg = AlgFactory::create_alg(this, DUMMY_ALG);
    setDisplayString("p=0,0");
    scheduleAt(0, new cMessage("Initialize the network!", SELF_INITIALIZE_MESSAGE));
}

void Network::handleMessage(cMessage *msg)
{
    EV << msg << '\n';
    if (msg->isSelfMessage() && msg->getKind() == SELF_INITIALIZE_MESSAGE) {
        buildNetwork(getParentModule());
        scheduleAt(0.5, new cMessage("Start Rounds", START_ROUND_MESSAGE));
    } else {
        alg->handle_message(msg);
    }
    delete msg;
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
    std::map<int, cModule *> nodeid2mod;
    std::string line;
    char modtypename[] = "node.Node";
    cModuleType *modtype = cModuleType::find(modtypename);
    if (!modtype)
        throw cRuntimeError("module type `%s' not found", modtypename);

    int n, m;
    std::fstream networkFile(par("networkFile").stringValue(), std::ios::in);
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
        nodeid2mod[nodeid] = mod;

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
        EV << "Edge " << srcnodeid << ' ' << destnodeid << '\n';

        if (nodeid2mod.find(srcnodeid) == nodeid2mod.end())
            throw cRuntimeError("wrong line in connections file: node with id=%d not found", srcnodeid);
        if (nodeid2mod.find(destnodeid) == nodeid2mod.end())
            throw cRuntimeError("wrong line in connections file: node with id=%d not found", destnodeid);

        cModule *srcmod = nodeid2mod[srcnodeid];
        cModule *destmod = nodeid2mod[destnodeid];

        cGate *srcIn, *srcOut, *destIn, *destOut;
        srcmod->getOrCreateFirstUnconnectedGatePair("port", false, true, srcIn, srcOut);
        destmod->getOrCreateFirstUnconnectedGatePair("port", false, true, destIn, destOut);

        // connect
        connect(srcOut, destIn);
        connect(destOut, srcIn);
    }

    // initialization will simply skip already-initialized modules instead of causing error
    EV << "Done buildNetwork " << parent->getName() << '\n';
    parent->callInitialize();
}

/**
 * @brief 
 * 
 * @return simtime_t 
 */
simtime_t Network::get_next_transmission_time() {
    return ROUND_TIME * ceil(simTime()/ROUND_TIME);
}

/**
 * @brief 
 * 
 */
void Network::send_synchrozied_message() {
    EV << "Network::send_synchrozied_message()\n";
    SynchronizedMessage *synchronized_message = new SynchronizedMessage("synchronized", SYNCHRONIZED_MESSAGE);
    synchronized_message->setSenderId(-1);
    synchronized_message->setRoundId(current_round_id);
    current_round_id += 1;
    synchronized_message->setSynchronizedMessageType(SYNCHRONIZED_START_ROUND);
    EV << "root msg: " << synchronized_message->getKind() << "\n";
    for(cGate *gate : nodesWirelessIn) {
        cMessage *msg = synchronized_message->dup();
        sendDirect(msg, gate);
        EV << "node " << gate->getOwner()->getName() << ' ' << msg->getKind() << '\n';
    }
    delete synchronized_message;
    //n_nodes_completed_in_current_round = 0;
}

Network::~Network() {
    delete alg;
}