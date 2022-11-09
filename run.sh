#!/bin/bash
# ./run.sh run/20/sparse/omnetpp_000.ini dummy
# ./run.sh run/20/sparse/omnetpp_000.ini MIS-Luby

omnetpp=${1:-"run/20/sparse/omnetpp_000.ini"}
config=${2:-"MIS-KW08"}
mode=${3:-"false"}

echo "omnetpp.ini = ${omnetpp}"
echo "config = ${config}"

./scds -f ${omnetpp} -n src -u Cmdenv --cmdenv-express-mode=${mode} -c ${config} -r 1
