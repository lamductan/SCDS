#ifndef SCDS_ALGORITHMS_CDS_GK96_CDS_GK96_CDS_H_
#define SCDS_ALGORITHMS_CDS_GK96_CDS_GK96_CDS_H_

#include "algorithms/cds/cds_simple/simple_cds.h"

class GK96CDSAlg : public SimpleCDSAlg
{
  public:
    int three_hop_path_construction_stage_starting_round = 0;
    int three_hop_path_construction_stage_max_num_rounds = 0;

    IAlgNode *three_hop_path_construction_alg = nullptr;

    virtual ~GK96CDSAlg();
};

#endif // SCDS_ALGORITHMS_CDS_GK96_CDS_GK96_CDS_H_