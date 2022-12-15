#!/bin/bash
# ./run.sh run/20/sparse/omnetpp_000.ini dummy
# ./run.sh run/20/sparse/omnetpp_000.ini MIS-Luby
# ./run.sh run/20/sparse/omnetpp_000.ini TwoRS-Lam > results/a.txt

mode=${3:-"false"}

n_tests=50
n_cores=25

f() {
  n=$1
  denseness=$2
  i=$3 
  j=`printf %03d $i`
  omnetpp="run/${n}/${denseness}/omnetpp_${j}.ini"
  #for config in 'MIS-Luby' 'MIS-SW08' 'MIS-GP22' 'MIS-Lam'
  for config in 'CDS-Simple-Lam'
  do
    command="./scds -f ${omnetpp} -n src -u Cmdenv --cmdenv-express-mode=${mode} -c ${config} -r 1 > /dev/null"
    echo ${command}
    eval ${command}
  done
}


#for n in {100..2000..100}
for n in {200..2000..200}
do
  denseness=sparse
  for ((i=0; i<${n_tests}; i+=${n_cores})); do
    for ((j=i; j < i+${n_cores}; j++)); do
      if (( $j >= ${n_tests} ))
      then
	break
      fi
      f $n $denseness $j &
    done
    wait
  done
done

cp -R results results_cds
zip -r results_cds.zip results_cds