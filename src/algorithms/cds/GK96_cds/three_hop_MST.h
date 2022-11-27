#ifndef SCDS_ALGORITHMS_CDS_GK96_CDS_THREE_HOP_MST_H_
#define SCDS_ALGORITHMS_CDS_GK96_CDS_THREE_HOP_MST_H_

#include "algorithms/mst/GHS_mst/GHS_MST.h"

class ThreeHopMSTAlg : public GHSMSTAlg
{
  public:
    std::map<int, std::array<int, 4>> nodes_on_path;
    virtual GHSMSTMessage *
    create_message(GHSMSTMessageType GHS_MST_message_type,
                   const std::tuple<int, int, int> &edge_id,
                   int relay_receiver_id = -1,
                   int real_sender_id = -1) override;
    virtual void process_async_message_queue() override;
    virtual void send_messages_current_round() override;
};

#endif // SCDS_ALGORITHMS_CDS_GK96_CDS_THREE_HOP_MST_H_