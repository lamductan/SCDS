#include "algorithms/ialg.h"

void IAlg::set_alg_type() { EV << "IAlg::set_alg_type()\n"; alg_type = DUMMY_ALG; }
    
void IAlg::set_max_num_rounds(int n_nodes) {
    if (alg_type == MIS_ALG) {
        max_num_rounds = 10*log(n_nodes)/log(2);
    } else if (alg_type == CDS_ALG) {
        max_num_rounds = 10*n_nodes*log(n_nodes)/log(2);
    } else {
        max_num_rounds = 3;
    }
    EV << "alg_type = " << alg_type << ", max_num_rounds = " << max_num_rounds << '\n';
}