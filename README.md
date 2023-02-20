# Graphitron
A powerful, high-performance language for graph processing
* version: 1.0
# Dependencies
* C++ compiler
* CMake 3.5.0 or greater
* Vitis 2019.2
* Xilinx_u280_xdma_201920_1
# Build Graphitron
1. build `graphitron compiler`.
```
    cd Graphitron
    mkdir build
    cd build
    cmake ..
    make
```
2. set xilinx environment variable
```
    source .../setup.sh
    source .../settings64.sh
```
3. compile a application
```
    make app=bfs all
```
4. load `dataset` and `xcl_bin` file, run application
```
    ./host_graph_fpga_bfs ./xclbin_bfs/graph_fpga.hw.xilinx_u280_xdma_201920_1.xclbin /Dataset/wiki-Talk.txt
```