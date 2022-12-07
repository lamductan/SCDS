#include "algorithms/cds/cds_simple/simple_cds_2.h"
#include "messages/synchronized_message.h"

void SimpleCDS2Alg::set_alg_type()
{
    EV << "SimpleCDS2Alg::set_alg_type()\n";
    alg_type = CDS_ALG;
}

void SimpleCDS2Alg::init_MIS()
{
    MIS_stage_starting_round = starting_round;
    MIS_stage_max_num_rounds = 45;
    Lam_MIS_alg = new LamMISAlg(node, MIS_stage_starting_round);
    Lam_MIS_alg->max_num_rounds = MIS_stage_max_num_rounds;
}

void SimpleCDS2Alg::init_CDS()
{
    CDS_stage_starting_round =
        MIS_stage_starting_round + MIS_stage_max_num_rounds;
    CDS_alg = new SimpleMISToCDSAlg(node, CDS_stage_starting_round);
    CDS_stage_max_num_rounds = CDS_alg->max_num_rounds;
}

SimpleCDS2Alg::SimpleCDS2Alg() {}

SimpleCDS2Alg::SimpleCDS2Alg(Node *node, int starting_round)
{
    init(node, starting_round);
    init_MIS();
    init_CDS();
    max_num_rounds = MIS_stage_max_num_rounds + CDS_stage_max_num_rounds;

    EV << "MIS_stage_starting_round = " << MIS_stage_starting_round << ' '
       << Lam_MIS_alg->starting_round << '\n';
    EV << "CDS_stage_starting_round = " << CDS_stage_starting_round << ' '
       << CDS_alg->starting_round << '\n';
    EV << "total max_num_rounds = " << max_num_rounds << '\n';
}

void SimpleCDS2Alg::handle_message(cMessage *msg)
{
    handle_synchronized_message(msg);
    EV << "current_round_id = " << current_round_id << "\n";
    switch (Simple_CDS_stage) {
    case SimpleCDS2Stage::MIS_STAGE:
        call_handle_message(Lam_MIS_alg, msg);
        break;
    case SimpleCDS2Stage::CDS_STAGE:
        call_handle_message(CDS_alg, msg);
        break;
    default:
        break;
    }

    n_awake_rounds = Lam_MIS_alg->n_awake_rounds + CDS_alg->n_awake_rounds;
    EV << "MIS_status after round #" << current_round_id << " = " << MIS_status
       << '\n';
    EV << "CDS_status after round #" << current_round_id << " = " << CDS_status
       << '\n';
    EV << "last_communication_round : " << last_communication_round << '\n';
}

void SimpleCDS2Alg::stage_transition()
{
    if (Simple_CDS_stage == SimpleCDS2Stage::INITIAL_STAGE) {
        Simple_CDS_stage = SimpleCDS2Stage::MIS_STAGE;
    } else if (current_round_id == CDS_stage_starting_round) {
        Simple_CDS_stage = SimpleCDS2Stage::CDS_STAGE;
        CDS_alg->MIS_status = Lam_MIS_alg->MIS_status;
        if (CDS_alg->MIS_status == IN_MIS)
            CDS_alg->CDS_status = TENTATIVE_IN_CDS;
    }
}

bool SimpleCDS2Alg::is_selected() { return (CDS_status == IN_CDS); }

SimpleCDS2Alg::~SimpleCDS2Alg()
{
    delete Lam_MIS_alg;
    delete CDS_alg;
}
