#ifndef SCDS_ALGORITHMS_CDS_LAM23_CDS_CONGEST_LAM23_CDS_CONGEST_H_
#define SCDS_ALGORITHMS_CDS_LAM23_CDS_CONGEST_LAM23_CDS_CONGEST_H_

#include "algorithms/cds/GK96_cds/GK96_CDS.h"

class LamCDSCongestAlg : public GK96CDSAlg
{
  public:
    LamCDSCongestAlg();
    LamCDSCongestAlg(Node *node, int starting_round = 1);
    virtual void init_CDS() override;
    virtual void init_three_hop_mst() override;
};

#endif // SCDS_ALGORITHMS_CDS_LAM23_CDS_CONGEST_LAM23_CDS_CONGEST_H_