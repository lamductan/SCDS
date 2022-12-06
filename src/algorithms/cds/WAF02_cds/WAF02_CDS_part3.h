#ifndef SCDS_ALGORITHMS_WAF02_CDS_WAF02_CDS_PART3_H_
#define SCDS_ALGORITHMS_WAF02_CDS_WAF02_CDS_PART3_H_

#include "algorithms/cds/WAF02_cds/WAF02_CDS_message.h"
#include "algorithms/cds/WAF02_cds/WAF02_TwoHopMIS.h"
#include "algorithms/ialg_node.h"

class WAF02CDSPart3Alg : public IAlgNode
{
  public:
    bool is_MIS_root = false;
    int parent_id = -1;
    int root = -1;
    int degree = 0;
    int x1;
    bool is_CDS_root = false;
    int z = 0;

    std::set<int> mis_neighbors;
    std::set<int> children;
    std::set<int> waiting_message_from;

    void MIS_root_send_first_QUERY_message();
    void process_QUERY_message(WAF02CDSMessage *WAF02_CDS_message);
    void process_REPORT_message(WAF02CDSMessage *WAF02_CDS_message);
    void process_ROOT_message(WAF02CDSMessage *WAF02_CDS_message);
    void process_INVITE1_message(WAF02CDSMessage *WAF02_CDS_message);
    void process_INVITE2_message(WAF02CDSMessage *WAF02_CDS_message);
    void process_JOIN_message(WAF02CDSMessage *WAF02_CDS_message);

  public:
    virtual void set_alg_type() override;
    WAF02CDSPart3Alg();
    void init_from_two_hop_MIS_alg(WAF02TwoHopMISAlg *WAF02_two_hop_MIS_alg);
    virtual void process_round() override;
    void process_WAF02_CDS_message_queue();
    void broadcast_message(WAF02CDSMessage *WAF02_CDS_message);
    void send_message_to(WAF02CDSMessage *WAF02_CDS_message, int receiver_id);
    void decide_CDS_status();
    virtual bool is_selected() override;
    virtual bool is_awake() override;
    virtual bool is_decided() override;
    WAF02CDSMessage *create_WAF02_CDS_message();

    virtual void print_state(int log_level = 0) override;
};

#endif // SCDS_ALGORITHMS_WAF02_CDS_WAF02_CDS_PART3_H_