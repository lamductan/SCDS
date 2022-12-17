# Implementation of the paper "Minimum Connected Dominating Set Construction in Wireless Sensor Networks under Sleeping Model"

## Requirement:
```
clang 14.0.0 (The code has been tested on Ubuntu 22.04.)
Omnet++
```

## Install Omnet++
```
sudo apt-get install build-essential clang lld gdb bison flex perl \
    python3 python3-pip qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools \
    libqt5opengl5-dev libxml2-dev zlib1g-dev doxygen graphviz libwebkit2gtk-4.0-37 -y

python3 -m pip install --user --upgrade numpy pandas matplotlib scipy seaborn posix_ipc
sudo apt-get install libopenscenegraph-dev zip -y
sudo apt-get install mpi-default-dev -y

wget https://github.com/omnetpp/omnetpp/releases/download/omnetpp-6.0.1/omnetpp-6.0.1-linux-x86_64.tgz
tar xvf omnetpp-6.0.1-linux-x86_64.tgz
cd omnetpp-6.0.1
rm -rf ide
rm -rf *.desktop
source setenv
./configure
echo "export PATH=$PATH:$PWD/bin" >> ~/.bashrc
source ~/.bashrc
make -j 8
```

## Download the code
```
git clone https://github.com/lamductan/SCDS
cd SCDS
chmod +x build.sh
chmod +x run.sh
chmod +x run_mis.sh
chmod +x run_cds.sh
```

## Build
```
./build.sh
```

## To run all simulations
```
./run_mis.sh
./run_cds.sh
```

Results will be saved in folders `results_mis` and `results_cds`.

Run the two Jupyter Notebook files `Stat_results_MIS.ipynb` and `Stat_results_CDS.ipynb` to plot the figures as in the paper.