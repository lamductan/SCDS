#!/bin/bash
This=scds

rm ${This} 2> /dev/null
cd src && opp_makemake -f --deep -o ${This} -O .. -I.
make
mv ${This} .. 2> /dev/null
cd ..

#./scds -f run/omnetpp.ini -n src -u Cmdenv --cmdenv-express-mode=false
