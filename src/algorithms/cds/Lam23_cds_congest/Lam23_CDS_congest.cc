#include "algorithms/cds/Lam23_cds_congest/Lam23_CDS_congest.h"
#include "algorithms/cds/Lam23_cds_congest/three_hop_AMP22_mst.h"
#include "utils/utils.h"

LamCDSCongestAlg::LamCDSCongestAlg() {}

LamCDSCongestAlg::LamCDSCongestAlg(Node *node, int starting_round)
{
    init(node, starting_round);
}

void LamCDSCongestAlg::init_CDS()
{
    CDS_stage_starting_round =
        three_hop_path_construction_stage_starting_round +
        three_hop_path_construction_stage_max_num_rounds;
    CDS_alg = new ThreeHopAMP22MSTAlg(node, CDS_stage_starting_round);
    CDS_stage_max_num_rounds = CDS_alg->max_num_rounds;
}

void LamCDSCongestAlg::init_three_hop_mst()
{
    EV << "LamCDSCongestAlg::init_three_hop_mst" << '\n';
    CDS_alg->MIS_status = MIS_status;
    CDS_alg->CDS_status = CDS_status;

    ThreeHopAMP22MSTAlg *CDS_alg_1 =
        dynamic_cast<ThreeHopAMP22MSTAlg *>(CDS_alg);
    SimpleMISToCDSAlg *three_hop_path_construction_alg_1 =
        dynamic_cast<SimpleMISToCDSAlg *>(three_hop_path_construction_alg);
    CDS_alg_1->nodes_on_path = three_hop_path_construction_alg_1->nodes_on_path;

    if (CDS_alg->MIS_status == NOT_IN_MIS) {
        if (three_hop_path_construction_alg_1->pos_of_relay_on_directed_paths
                .empty())
            return;
        CDS_alg_1->pos_of_relay_on_directed_paths =
            three_hop_path_construction_alg_1->pos_of_relay_on_directed_paths;
        std::vector<std::tuple<int, int, int>> inter_fragment_directed_edges =
            getMapKeys<std::tuple<int, int, int>, int>(
                CDS_alg_1->pos_of_relay_on_directed_paths);
        CDS_alg_1->inter_fragment_directed_edges =
            std::set<std::tuple<int, int, int>>(
                inter_fragment_directed_edges.begin(),
                inter_fragment_directed_edges.end());

        std::tuple<int, int, int> longest_path =
            *(CDS_alg_1->inter_fragment_directed_edges.rbegin());
        CDS_alg_1->on_w_2_path = (std::get<0>(longest_path) == 2);
        for (const auto &it : CDS_alg_1->pos_of_relay_on_directed_paths) {
            std::tuple<int, int, int> edge = it.first;
            int u = std::get<1>(edge);
            int v = std::get<2>(edge);
            CDS_alg_1->fragment_informations[u] = { u, 0, -1 };
            CDS_alg_1->fragment_informations[v] = { v, 0, -1 };
        }
        return;
    }

    EV << "\tCDS_alg_1->starting_round = " << CDS_alg_1->starting_round << '\n';

    CDS_alg_1->neighbors_set.clear();
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
        CDS_alg_1->edges_map[two_hop_MIS_neighbor] = edge;
        CDS_alg_1->edges_id_map[two_hop_MIS_neighbor] = edge_id;
        CDS_alg_1->neighbors_set.insert(two_hop_MIS_neighbor);
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
        CDS_alg_1->edges_map[three_hop_MIS_neighbor] = edge;
        CDS_alg_1->edges_id_map[three_hop_MIS_neighbor] = edge_id;
        CDS_alg_1->neighbors_set.insert(three_hop_MIS_neighbor);
    }

    CDS_alg_1->different_fragment_neighbors = CDS_alg_1->neighbors_set;
    CDS_alg_1->node_moe = CDS_alg_1->fragment_moe =
        (CDS_alg_1->basic_edges_set.empty())
            ? INF_EDGE_ID
            : *CDS_alg_1->basic_edges_set.begin();
}