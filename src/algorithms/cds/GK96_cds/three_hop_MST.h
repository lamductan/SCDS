#ifndef SCDS_ALGORITHMS_CDS_GK96_CDS_THREE_HOP_MST_H_
#define SCDS_ALGORITHMS_CDS_GK96_CDS_THREE_HOP_MST_H_

#include "algorithms/mst/GHS_mst/GHS_MST.h"

class ThreeHopMSTAlg : public GHSMSTAlg
{
  public:
    std::map<int, std::array<int, 4>> nodes_on_path;

  public:
    ThreeHopMSTAlg(Node *node, int starting_round = 1);
    virtual void init(Node *node, int starting_round) override;
    virtual int
    find_next_relay_node_id_on_path_to(int next_node_on_path_id) override;
    virtual void process_async_message_queue() override;
    virtual bool is_awake() override;
};

#endif // SCDS_ALGORITHMS_CDS_GK96_CDS_THREE_HOP_MST_H_