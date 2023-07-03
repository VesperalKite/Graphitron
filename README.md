# Graphitron
A powerful, high-performance language for graph processing
# Dependencies
* The gcc-8.3 or above
* The CMake-3.5.0 or above
* Tools:
    * Vitis 2019.2
* Evaluated platforms from Xilinx:
    * Alveo U50  Data Center Accelerator Card
    * Alveo U280 Data Center Accelerator Card
## Build Graphitron
After you have cloned the directory:
```
    cd Graphitron
    mkdir build
    cd build
    cmake ..
    make
```
## Compile Graphitron Programs
#### Here is the example of implementing the accelerator for BFS on Alveo U280 platform with Vitis 2019.2
```
# set xilinx environment variable
    source .../setup.sh
    source .../settings64.sh
# build kernel and host application
    make app=bfs all
# delpoy fpga and run task
    ./host_graph_fpga_bfs $(DATASET)/wiki-Talk.el
```
## Optimize Option
#### To obtain better performance, we introduce a GAS-model version
```
    make app=pagerank_delta all opt=1
```
opt defalut will be set as 0