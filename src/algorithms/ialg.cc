#include "algorithms/ialg.h"
#include "utils/utils.h"

void IAlg::set_alg_type()
{
    EV << "IAlg::set_alg_type()\n";
    alg_type = DUMMY_ALG;
}

void IAlg::set_max_num_rounds(int n_nodes)
{
    if (alg_type == MIS_ALG || alg_type == TWO_RS_ALG) {
        max_num_rounds = 3 * log_2(n_nodes);
    } else if (alg_type == CDS_ALG || alg_type == MST_ALG) {
        max_num_rounds = 3 * n_nodes * log_2(n_nodes);
    } else {
        max_num_rounds = 3;
    }
    // max_num_rounds = 100; //FOR DEBUG PURPOSE ONLY
    EV << "alg_type = " << alg_type << ", max_num_rounds = " << max_num_rounds
       << '\n';
}