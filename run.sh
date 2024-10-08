#!/bin/bash
# ./run.sh run/20/sparse/omnetpp_000.ini dummy
# ./run.sh run/20/sparse/omnetpp_000.ini MIS-Luby
# ./run.sh run/20/sparse/omnetpp_000.ini TwoRS-Lam > results/a4.txt
# ./run.sh run/1900/sparse/omnetpp_000.ini TwoRS-BGKO22 > results/a4.txt
# ./run.sh run/1900/sparse/omnetpp_000.ini MIS-Lam > results/a4.txt

# ./run.sh run/20/sparse/omnetpp_000.ini CDS-Lam > results/a4.txt
# ./run.sh run/20/sparse/omnetpp_000.ini MST-GHS > results/a4.txt
# ./run.sh run/20/sparse/omnetpp_000.ini BFS > results/a4.txt
# ./run.sh run/20/sparse/omnetpp_000.ini BFS-Lam-Local > results/a4.txt
# ./run.sh run/20/sparse/omnetpp_000.ini MST-AMP22 > results/a4.txt
# ./run.sh run/20/sparse/omnetpp_000.ini CDS-Lam-Congest > results/a4.txt

omnetpp=${1:-"run/20/sparse/omnetpp_000.ini"}
config=${2:-"MIS-KW08"}
mode="false"
r=${3:-"1"}

echo "omnetpp.ini = ${omnetpp}"
echo "config = ${config}"

./scds -f ${omnetpp} -n src -u Cmdenv --allow-object-stealing-on-deletion=true --cmdenv-express-mode=${mode} -c ${config} -r ${r}
