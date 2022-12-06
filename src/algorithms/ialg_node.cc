#include "algorithms/ialg_node.h"
#include "messages/messages.h"

using namespace omnetpp;

void IAlgNode::init(Node *node, int starting_round)
{
    this->starting_round = starting_round;
    this->node = node;
    this->id = node->id;
    this->n_nodes = node->n_nodes;
    set_alg_type();
    set_max_num_rounds(this->n_nodes);
    current_round_alg_stage = BaseAlgStage::INITIAL_STAGE;
    previous_round_alg_stage = BaseAlgStage::INITIAL_STAGE;
    neighbors_set = need_to_send =
        std::set<int>(node->all_neighbors.begin(), node->all_neighbors.end());
}

IAlgNode::IAlgNode() {}

IAlgNode::IAlgNode(Node *node, int starting_round)
{
    init(node, starting_round);
}

bool IAlgNode::is_selected() { return true; }

bool IAlgNode::is_awake()
{
    if (!is_decided()) {
        awake_round_map[current_round_id] = true;
        return true;
    }
    if (current_round_id <= decided_round) {
        awake_round_map[current_round_id] = true;
        return true;
    }
    return false;
}

void IAlgNode::handle_message(cMessage *msg)
{
    if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
        SynchronizedMessage *synchronized_message =
            dynamic_cast<SynchronizedMessage *>(msg);
        if (synchronized_message->getSynchronizedMessageType() ==
            SYNCHRONIZED_START_ROUND) {
            start_round(msg);
            update_previous_status();
            process_round();
            record_decided_round();
        } else {
            delete msg;
        }
    } else {
        listen_new_message(msg);
    }
}

void IAlgNode::record_decided_round()
{
    current_decided_status = is_decided();
    if (!previous_decided_status && current_decided_status) {
        decided_round = current_round_id;
    }
    previous_decided_status = current_decided_status;
}

void IAlgNode::start_round(cMessage *msg)
{
    SynchronizedMessage *synchronized_message =
        dynamic_cast<SynchronizedMessage *>(msg);
    int sender_id = synchronized_message->getSenderId();
    assert(sender_id == -1);
    current_round_id = synchronized_message->getRoundId();
    synchronized_message_ptr = msg;
    // EV << "node" << node->id
    //    << " received synchronized message of round " << current_round_id <<
    //    "\n";
    delete msg;
    synchronized_message_ptr = nullptr;
}

void IAlgNode::process_round()
{
    EV << "IAlgNode::process_round()\n";
    stage_transition();
    EV << "n_awake_rounds = " << n_awake_rounds << '\n';
    if (!is_awake()) {
        EV << "not awake\n";
        return;
    }
    ++n_awake_rounds;
    awake_round_map[current_round_id] = true;
    EV << "Is awake: n_awake_rounds = " << n_awake_rounds << '\n';
    EV << "pre_process: message_queue.size() = " << message_queue.size()
       << '\n';
    cMessage *new_message = process_message_queue();
    print_state();
    clear_message_queue();
    if (new_message != nullptr) send_new_message(new_message);
}

void IAlgNode::handle_synchronized_message(cMessage *msg)
{
    if (msg->getKind() == SYNCHRONIZED_MESSAGE) {
        SynchronizedMessage *synchronized_message =
            dynamic_cast<SynchronizedMessage *>(msg);
        if (synchronized_message->getSynchronizedMessageType() ==
            SYNCHRONIZED_START_ROUND) {
            synchronized_message_ptr = msg;
            current_round_id = synchronized_message->getRoundId();
            stage_transition();
        }
    }
}

void IAlgNode::stage_transition()
{
    if (current_round_id - starting_round >= max_num_rounds) {
        current_round_alg_stage = BaseAlgStage::END_STAGE;
        return;
    }
}

void IAlgNode::clear_message_queue()
{
    for (cMessage *msg : message_queue) delete msg;
    message_queue.clear();
}

cMessage *IAlgNode::process_message_queue() { return nullptr; }

void IAlgNode::send_new_message(cMessage *msg, double delay)
{
    EV << "node" << node->id << " : need_to_send = [";
    for (int x : need_to_send) EV << x << ",";
    EV << "]\n";

    last_communication_round = current_round_id;
    if (need_to_send.size() == 1 && *need_to_send.begin() == -1) {
        /*
        int n_neighbors = node->gateSize("port");
        for(int i = 0; i < n_neighbors; ++i) {
            node->sendDelayed(msg->dup(), delay, "port$o", i);
        }
        */
        for (int neighbor_id : node->all_neighbors) {
            node->sendDelayed(msg->dup(), delay,
                              node->neighbor_gates[neighbor_id]);
        }
    } else {
        for (int neighbor_id : need_to_send) {
            node->sendDelayed(msg->dup(), delay,
                              node->neighbor_gates[neighbor_id]);
        }
    }
    delete msg;
}

void IAlgNode::listen_new_message(cMessage *msg)
{
    EV << "is_awake() = " << is_awake() << '\n';
    if (!is_awake())
        delete msg;
    else if (alg_type == MIS_ALG && is_decided())
        delete msg;
    else {
        message_queue.push_back(msg);
        last_communication_round = current_round_id;
        EV << "is_awake() = " << is_awake() << ' '
           << "message_queue.size() = " << message_queue.size() << '\n';
    }
}

bool IAlgNode::is_decided() { return (MIS_status != UNDECIDED); }

void IAlgNode::update_previous_status() { previous_MIS_status = MIS_status; }

void IAlgNode::call_handle_message(IAlgNode *alg, cMessage *msg)
{
    update_previous_status();
    alg->handle_message(msg);
    MIS_status = alg->MIS_status;
    dominator = alg->dominator;
    awake_round_map[current_round_id] = alg->awake_round_map[current_round_id];
    if (CDS_status != IN_CDS) CDS_status = alg->CDS_status;
    if (is_decided() && alg->decided_round > -1)
        decided_round = alg->decided_round;
    if (alg->last_communication_round > -1)
        last_communication_round = alg->last_communication_round;
}

void IAlgNode::delete_synchronized_message()
{
    delete synchronized_message_ptr;
}

void IAlgNode::print_state(int log_level) {}

centralized::BFSTreeStructure IAlgNode::get_bfs_tree_structure() const
{
    return centralized::BFSTreeStructure();
}

IAlgNode::~IAlgNode() { clear_message_queue(); }

std::string MIS_status_to_string(MISNodeStatus MIS_status)
{
    switch (MIS_status) {
    case UNDECIDED:
        return "UNDECIDED";
    case IN_MIS:
        return "IN_MIS";
    case NOT_IN_MIS:
        return "NOT_IN_MIS";
    default:
        break;
    }
}

std::string CDS_status_to_string(CDSNodeStatus CDS_status)
{
    switch (CDS_status) {
    case NOT_IN_CDS:
        return "NOT_IN_CDS";
    case TENTATIVE_IN_CDS:
        return "TENTATIVE_IN_CDS";
    case IN_CDS:
        return "IN_CDS";
    default:
        break;
    }
}