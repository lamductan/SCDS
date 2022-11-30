#include "algorithms/cds/WAF02_cds/WAF02_TwoHopMIS.h"
#include "messages/synchronized_message.h"

void WAF02TwoHopMISAlg::set_alg_type()
{
    EV << "WAF02TwoHopMISAlg::set_alg_type()\n";
    alg_type = TWO_HOP_MIS_ALG;
}

WAF02TwoHopMISAlg::WAF02TwoHopMISAlg(Node *node, int starting_round)
{
    init(node, starting_round);
    bfs_starting_round = starting_round;
    bfs_alg = new BFSAlg(node, bfs_starting_round);
    WAF02_TwoHopMIS_part2_alg = new WAF02TwoHopMISPart2Alg();
}

void WAF02TwoHopMISAlg::handle_message(cMessage *msg)
{
    handle_synchronized_message(msg);
    EV << "current_round_id = " << current_round_id << "\n";
    EV << "WAF02_TwoHopMIS_stage = " << WAF02_TwoHopMIS_stage << "\n";
    switch (WAF02_TwoHopMIS_stage) {
    case WAF02TwoHopMISStage::BFS_STAGE:
        call_handle_message(bfs_alg, msg);
        break;
    case WAF02TwoHopMISStage::TWO_HOP_MIS_STAGE:
        call_handle_message(WAF02_TwoHopMIS_part2_alg, msg);
        break;
    default:
        break;
    }

    n_awake_rounds =
        bfs_alg->n_awake_rounds + WAF02_TwoHopMIS_part2_alg->n_awake_rounds;
    EV << "MIS_status after round #" << current_round_id << " = " << MIS_status
       << '\n';
    EV << "last_communication_round : " << last_communication_round << '\n';
}

void WAF02TwoHopMISAlg::stage_transition()
{
    EV << "bfs_alg->is_completed = " << bfs_alg->is_completed << '\n';
    if (WAF02_TwoHopMIS_stage == WAF02TwoHopMISStage::INITIAL_STAGE) {
        WAF02_TwoHopMIS_stage = WAF02TwoHopMISStage::BFS_STAGE;
    } else if (bfs_alg->is_completed &&
               WAF02_TwoHopMIS_stage == WAF02TwoHopMISStage::BFS_STAGE) {
        WAF02_TwoHopMIS_stage = WAF02TwoHopMISStage::TWO_HOP_MIS_STAGE;
        WAF02_TwoHopMIS_part2_alg->init_from_bfs_alg(bfs_alg);
    }
}

bool WAF02TwoHopMISAlg::is_awake()
{
    switch (WAF02_TwoHopMIS_stage) {
    case WAF02TwoHopMISStage::BFS_STAGE:
        return bfs_alg->is_awake();
    case WAF02TwoHopMISStage::TWO_HOP_MIS_STAGE:
        return WAF02_TwoHopMIS_part2_alg->is_awake();
    default:
        return true;
    }
}

bool WAF02TwoHopMISAlg::is_decided()
{
    if (WAF02_TwoHopMIS_part2_alg == nullptr) return false;
    return WAF02_TwoHopMIS_part2_alg->is_decided();
}

bool WAF02TwoHopMISAlg::is_selected() { return MIS_status == IN_MIS; }

WAF02TwoHopMISAlg::~WAF02TwoHopMISAlg()
{
    delete bfs_alg;
    delete WAF02_TwoHopMIS_part2_alg;
}