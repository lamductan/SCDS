#include "algorithms/cds/WAF02_cds/WAF02_CDS.h"
#include "messages/synchronized_message.h"

void WAF02CDSAlg::set_alg_type()
{
    EV << "WAF02CDSAlg::set_alg_type()\n";
    alg_type = CDS_ALG;
}

WAF02CDSAlg::WAF02CDSAlg(Node *node, int starting_round)
{
    init(node, starting_round);
    WAF02_TwoHopMIS_starting_round = starting_round;
    WAF02_TwoHopMIS_alg =
        new WAF02TwoHopMISAlg(node, WAF02_TwoHopMIS_starting_round);
    WAF02_CDS_part3_alg = new WAF02CDSPart3Alg();
}

void WAF02CDSAlg::handle_message(cMessage *msg)
{
    handle_synchronized_message(msg);
    EV << "current_round_id = " << current_round_id << "\n";
    EV << "WAF02_CDS_stage = " << WAF02_CDS_stage << "\n";
    switch (WAF02_CDS_stage) {
    case WAF02CDSStage::TWO_HOP_MIS_STAGE:
        call_handle_message(WAF02_TwoHopMIS_alg, msg);
        break;
    case WAF02CDSStage::CDS_STAGE:
        call_handle_message(WAF02_CDS_part3_alg, msg);
        break;
    default:
        break;
    }

    n_awake_rounds = WAF02_TwoHopMIS_alg->n_awake_rounds +
                     WAF02_CDS_part3_alg->n_awake_rounds;
    EV << "MIS_status after round #" << current_round_id << " = " << MIS_status
       << '\n';
    EV << "CDS_status after round #" << current_round_id << " = " << CDS_status
       << '\n';
    EV << "last_communication_round : " << last_communication_round << '\n';
}

void WAF02CDSAlg::stage_transition()
{
    if (WAF02_CDS_stage == WAF02CDSStage::INITIAL_STAGE) {
        WAF02_CDS_stage = WAF02CDSStage::TWO_HOP_MIS_STAGE;
    } else if (WAF02_TwoHopMIS_alg->is_decided() &&
               WAF02_CDS_stage == WAF02CDSStage::TWO_HOP_MIS_STAGE) {
        WAF02_CDS_stage = WAF02CDSStage::CDS_STAGE;
        WAF02_CDS_part3_alg->init_from_two_hop_MIS_alg(WAF02_TwoHopMIS_alg);
    }
}

bool WAF02CDSAlg::is_awake()
{
    switch (WAF02_CDS_stage) {
    case WAF02CDSStage::TWO_HOP_MIS_STAGE:
        return WAF02_TwoHopMIS_alg->is_awake();
    case WAF02CDSStage::CDS_STAGE:
        return WAF02_CDS_part3_alg->is_awake();
    default:
        return true;
    }
}

bool WAF02CDSAlg::is_decided()
{
    if (WAF02_CDS_part3_alg == nullptr) return false;
    return WAF02_CDS_part3_alg->is_decided();
}

bool WAF02CDSAlg::is_selected() { return CDS_status == IN_CDS; }

WAF02CDSAlg::~WAF02CDSAlg()
{
    delete WAF02_TwoHopMIS_alg;
    delete WAF02_CDS_part3_alg;
}