#!/bin/bash
# ./run.sh run/20/sparse/omnetpp_000.ini dummy
# ./run.sh run/20/sparse/omnetpp_000.ini MIS-Luby
# ./run.sh run/20/sparse/omnetpp_000.ini TwoRS-Lam > results/a4.txt
# ./run.sh run/1900/sparse/omnetpp_000.ini TwoRS-BGKO22 > results/a4.txt
# ./run.sh run/1900/sparse/omnetpp_000.ini MIS-Lam > results/a4.txt

# ./run.sh run/20/sparse/omnetpp_000.ini CDS-Lam > results/a4.txt
# ./run.sh run/20/sparse/omnetpp_000.ini MST-GHS > results/a4.txt

omnetpp=${1:-"run/20/sparse/omnetpp_000.ini"}
config=${2:-"MIS-KW08"}
mode=${3:-"false"}

echo "omnetpp.ini = ${omnetpp}"
echo "config = ${config}"

./scds -f ${omnetpp} -n src -u Cmdenv --cmdenv-express-mode=${mode} -c ${config} -r 1
