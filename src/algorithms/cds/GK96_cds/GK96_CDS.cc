#include "algorithms/cds/GK96_cds/GK96_CDS.h"
#include "algorithms/cds/GK96_cds/three_hop_MST.h"
#include "algorithms/cds/cds_simple/simple_cds_message.h"
#include "algorithms/cds/cds_simple/simple_mis_to_cds.h"
#include "messages/synchronized_message.h"
#include "utils/utils.h"

GK96CDSAlg::GK96CDSAlg() {}

GK96CDSAlg::GK96CDSAlg(Node *node, int starting_round)
{
    init(node, starting_round);
}

void GK96CDSAlg::init(Node *node, int starting_round)
{
    IAlgNode::init(node, starting_round);
    init_MIS();
    init_three_hop_path_construction_alg();
    init_CDS();
    max_num_rounds = MIS_stage_max_num_rounds +
                     three_hop_path_construction_stage_max_num_rounds +
                     CDS_stage_max_num_rounds;
    CDS_status = NOT_IN_CDS;

    EV << "MIS_stage_starting_round = " << MIS_stage_starting_round << ' '
       << SW08_MIS_alg->starting_round << '\n';
    EV << "three_hop_path_construction_stage_starting_round = "
       << three_hop_path_construction_stage_starting_round << ' '
       << three_hop_path_construction_alg->starting_round << '\n';
    EV << "CDS_stage_starting_round = " << CDS_stage_starting_round << ' '
       << CDS_alg->starting_round << '\n';
    EV << "total max_num_rounds = " << max_num_rounds << '\n';
}

void GK96CDSAlg::init_three_hop_path_construction_alg()
{
    three_hop_path_construction_stage_starting_round =
        MIS_stage_starting_round + MIS_stage_max_num_rounds;
    three_hop_path_construction_alg = new SimpleMISToCDSAlg(
        node, three_hop_path_construction_stage_starting_round);
    three_hop_path_construction_stage_max_num_rounds = 6;
    three_hop_path_construction_alg->max_num_rounds =
        three_hop_path_construction_stage_max_num_rounds;
}

void GK96CDSAlg::init_CDS()
{
    CDS_stage_starting_round =
        three_hop_path_construction_stage_starting_round +
        three_hop_path_construction_stage_max_num_rounds;
    CDS_alg = new ThreeHopMSTAlg(node, CDS_stage_starting_round);
    CDS_stage_max_num_rounds = CDS_alg->max_num_rounds;
}

void GK96CDSAlg::handle_message(cMessage *msg)
{
    handle_synchronized_message(msg);
    EV << "current_round_id = " << current_round_id << "\n";
    switch (GK96_CDS_stage) {
    case GK96CDSStage::MIS_STAGE:
        call_handle_message(SW08_MIS_alg, msg);
        break;
    case GK96CDSStage::THREE_HOP_PATH_CONSTRUCTION_STAGE:
        call_handle_message(three_hop_path_construction_alg, msg);
        if (SW08_MIS_alg->MIS_status == IN_MIS) {
            // A trick: temporarily mark MIS nodes out of CDS
            CDS_status = TENTATIVE_IN_CDS;
        } else if (three_hop_path_construction_alg->CDS_status == IN_CDS) {
            CDS_status = TENTATIVE_IN_CDS;
        }
        break;
    case GK96CDSStage::CDS_STAGE:
        call_handle_message(CDS_alg, msg);
        if (CDS_alg->MIS_status == IN_MIS && CDS_alg->is_decided()) {
            // MST algorithm is done. Add MIS nodes back to CDS
            CDS_status = CDS_alg->CDS_status = IN_CDS;
        }
        break;
    default:
        break;
    }

    n_awake_rounds = SW08_MIS_alg->n_awake_rounds +
                     three_hop_path_construction_alg->n_awake_rounds +
                     CDS_alg->n_awake_rounds;
    EV << "MIS_status after round #" << current_round_id << " = " << MIS_status
       << '\n';
    EV << "CDS_status after round #" << current_round_id << " = " << CDS_status
       << '\n';
    EV << "last_communication_round : " << last_communication_round << '\n';
}

void GK96CDSAlg::stage_transition()
{
    if (GK96_CDS_stage == GK96CDSStage::INITIAL_STAGE) {
        GK96_CDS_stage = GK96CDSStage::MIS_STAGE;
    } else if (current_round_id ==
               three_hop_path_construction_stage_starting_round) {
        GK96_CDS_stage = GK96CDSStage::THREE_HOP_PATH_CONSTRUCTION_STAGE;
        three_hop_path_construction_alg->MIS_status = SW08_MIS_alg->MIS_status;
    } else if (current_round_id == CDS_stage_starting_round) {
        if (CDS_status != TENTATIVE_IN_CDS) {
            GK96_CDS_stage = GK96CDSStage::END_STAGE;
            decided_round = current_round_id - 1;
            node->is_finished = true;
        } else {
            GK96_CDS_stage = GK96CDSStage::CDS_STAGE;
            init_three_hop_mst();
        }
    }
}

void GK96CDSAlg::init_three_hop_mst()
{
    EV << "GK96CDSAlg::init_three_hop_mst" << '\n';
    CDS_alg->MIS_status = MIS_status;
    CDS_alg->CDS_status = CDS_status;

    ThreeHopMSTAlg *CDS_alg_1 = dynamic_cast<ThreeHopMSTAlg *>(CDS_alg);
    SimpleMISToCDSAlg *three_hop_path_construction_alg_1 =
        dynamic_cast<SimpleMISToCDSAlg *>(three_hop_path_construction_alg);
    CDS_alg_1->nodes_on_path = three_hop_path_construction_alg_1->nodes_on_path;

    if (CDS_alg->MIS_status == NOT_IN_MIS) {
        return;
    }

    for (int two_hop_MIS_neighbor : getMapKeys<int, std::array<int, 3>>(
             three_hop_path_construction_alg_1->MIS_in_two_hop_neighbor)) {
        centralized::Edge edge(id, two_hop_MIS_neighbor, 1);
        std::tuple<int, int, int> edge_id = edge.get_id();
        EV << "\tTwo-hop: " << tuple_to_string(edge_id) << '\n';
        EV << "\t\tNodes on path = [";
        for (int x : CDS_alg_1->nodes_on_path[two_hop_MIS_neighbor])
            EV << x << ",";
        EV << "]\n";
        CDS_alg_1->basic_edges_set.insert(edge_id);
        CDS_alg_1->edges_map[edge_id] = edge;
        CDS_alg_1->edges_state_map[edge_id] = GHS_MST_EDGE_STATE_BASIC;
    }

    for (int three_hop_MIS_neighbor : getMapKeys<int, std::array<int, 3>>(
             three_hop_path_construction_alg_1->MIS_in_three_hop_neighbor)) {
        centralized::Edge edge(id, three_hop_MIS_neighbor, 2);
        std::tuple<int, int, int> edge_id = edge.get_id();
        EV << "\tThree-hop: " << tuple_to_string(edge_id) << '\n';
        EV << "\t\tNodes on path = [";
        for (int x : CDS_alg_1->nodes_on_path[three_hop_MIS_neighbor])
            EV << x << ",";
        EV << "]\n";
        CDS_alg_1->basic_edges_set.insert(edge_id);
        CDS_alg_1->edges_map[edge_id] = edge;
        CDS_alg_1->edges_state_map[edge_id] = GHS_MST_EDGE_STATE_BASIC;
    }

    CDS_alg_1->fragment_core = { INF_EDGE_ID, id };
    CDS_alg_1->minimum_outgoing_edge_id = *CDS_alg_1->basic_edges_set.begin();
    CDS_alg_1->add_branch_edge(CDS_alg_1->minimum_outgoing_edge_id);
}

bool GK96CDSAlg::is_awake()
{
    switch (GK96_CDS_stage) {
    case GK96CDSStage::MIS_STAGE:
        return SW08_MIS_alg->is_awake();
    case GK96CDSStage::THREE_HOP_PATH_CONSTRUCTION_STAGE:
        return true;
    case GK96CDSStage::CDS_STAGE:
        return CDS_alg->is_awake();
    default:
        return true;
    }
}

bool GK96CDSAlg::is_decided()
{
    if (GK96_CDS_stage != GK96CDSStage::CDS_STAGE) return false;
    if (MIS_status == IN_MIS) return CDS_alg->is_decided();
    return true;
}

bool GK96CDSAlg::is_selected() { return CDS_status == IN_CDS; }

GK96CDSAlg::~GK96CDSAlg() { delete three_hop_path_construction_alg; }
