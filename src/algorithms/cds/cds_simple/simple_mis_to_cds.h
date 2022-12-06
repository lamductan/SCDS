#ifndef SCDS_ALGORITHMS_CDS_SIMPLE_MIS_TO_CDS_H_
#define SCDS_ALGORITHMS_CDS_SIMPLE_MIS_TO_CDS_H_

#include "algorithms/ialg_node.h"

class SimpleMISToCDSAlg : public IAlgNode
{
  public:
    std::map<int, std::array<int, 3>> MIS_in_one_hop_neighbor;
    std::map<int, std::array<int, 3>> MIS_in_two_hop_neighbor;
    std::map<int, std::array<int, 3>> MIS_in_three_hop_neighbor;
    std::map<int, std::array<int, 3>> MIS_in_two_three_hop_neighbor;
    std::map<int, std::array<int, 4>> nodes_on_path;
    std::map<std::tuple<int, int, int>, std::array<int, 4>>
        nodes_on_directed_paths;
    std::map<std::tuple<int, int, int>, int> pos_of_relay_on_directed_paths;

    cMessage *process_message_queue_exchange_MIS_status();
    cMessage *process_message_queue_exchange_one_hop_MIS_neighbors();
    cMessage *process_message_queue_exchange_two_hop_MIS_neighbors();
    cMessage *process_message_queue_MIS_nodes_inform_one_hop_neighbors();
    cMessage *
    process_message_queue_one_hop_MIS_neighbors_inform_two_hop_MIS_neighbors();
    cMessage *process_message_queue_two_hop_MIS_neighbors();

    std::map<std::tuple<int, int, int>, int>
    extract_pos_of_relay_on_directed_paths(
        const std::map<std::tuple<int, int, int>, std::array<int, 4>>
            &nodes_on_directed_paths) const;

    int
    extract_pos_of_relay_on_directed_path(const std::array<int, 4> &path) const;

  public:
    virtual void set_alg_type() override;
    SimpleMISToCDSAlg(Node *node, int starting_round = 1);
    virtual cMessage *process_message_queue() override;
    virtual bool is_selected() override;
    virtual bool is_awake() override;
};

#endif // SCDS_ALGORITHMS_CDS_SIMPLE_MIS_TO_CDS_H_