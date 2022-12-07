#include "algorithms/cds/cds_simple/simple_cds.h"
#include "messages/synchronized_message.h"

void SimpleCDSAlg::set_alg_type()
{
    EV << "SimpleCDSAlg::set_alg_type()\n";
    alg_type = CDS_ALG;
}

void SimpleCDSAlg::init_MIS()
{
    MIS_stage_starting_round = starting_round;
    MIS_stage_max_num_rounds = 20;
    SW08_MIS_alg = new SW08MISAlg(node, MIS_stage_starting_round);
    SW08_MIS_alg->max_num_rounds = MIS_stage_max_num_rounds;
}

void SimpleCDSAlg::init_CDS()
{
    CDS_stage_starting_round =
        MIS_stage_starting_round + MIS_stage_max_num_rounds;
    CDS_alg = new SimpleMISToCDSAlg(node, CDS_stage_starting_round);
    CDS_stage_max_num_rounds = CDS_alg->max_num_rounds;
}

SimpleCDSAlg::SimpleCDSAlg() {}

SimpleCDSAlg::SimpleCDSAlg(Node *node, int starting_round)
{
    init(node, starting_round);
    init_MIS();
    init_CDS();
    max_num_rounds = MIS_stage_max_num_rounds + CDS_stage_max_num_rounds;

    EV << "MIS_stage_starting_round = " << MIS_stage_starting_round << ' '
       << SW08_MIS_alg->starting_round << '\n';
    EV << "CDS_stage_starting_round = " << CDS_stage_starting_round << ' '
       << CDS_alg->starting_round << '\n';
    EV << "total max_num_rounds = " << max_num_rounds << '\n';
}

void SimpleCDSAlg::handle_message(cMessage *msg)
{
    handle_synchronized_message(msg);
    EV << "current_round_id = " << current_round_id << "\n";
    switch (Simple_CDS_stage) {
    case SimpleCDSStage::MIS_STAGE:
        call_handle_message(SW08_MIS_alg, msg);
        break;
    case SimpleCDSStage::CDS_STAGE:
        call_handle_message(CDS_alg, msg);
        break;
    default:
        break;
    }

    n_awake_rounds = SW08_MIS_alg->n_awake_rounds + CDS_alg->n_awake_rounds;
    EV << "MIS_status after round #" << current_round_id << " = " << MIS_status
       << '\n';
    EV << "CDS_status after round #" << current_round_id << " = " << CDS_status
       << '\n';
    EV << "last_communication_round : " << last_communication_round << '\n';
}

void SimpleCDSAlg::stage_transition()
{
    if (Simple_CDS_stage == SimpleCDSStage::INITIAL_STAGE) {
        Simple_CDS_stage = SimpleCDSStage::MIS_STAGE;
    } else if (current_round_id == CDS_stage_starting_round) {
        Simple_CDS_stage = SimpleCDSStage::CDS_STAGE;
        CDS_alg->MIS_status = SW08_MIS_alg->MIS_status;
        if (CDS_alg->MIS_status == IN_MIS)
            CDS_alg->CDS_status = TENTATIVE_IN_CDS;
    }
}

bool SimpleCDSAlg::is_selected() { return (CDS_status == IN_CDS); }

SimpleCDSAlg::~SimpleCDSAlg()
{
    delete SW08_MIS_alg;
    delete CDS_alg;
}
