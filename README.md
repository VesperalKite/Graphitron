# Graphitron
A powerful, high-performance language for graph processing
* version: 0. 0
# Dependencies
* C++ compiler
* CMake 3.5.0 or greater
* Python 2.7 and Python 3
* Vitis 2019.2
* xilinx_u280_xdma_201920_1
# Build Graphitron
1. make a DSL output directory.
```
    cd ThunderGP/application
    mkdir graphitpr
```
2. build a DSL compiler.
```
    cd GraphIt
    mkdir build
    cd build
    cmake ..
    make
```
3. read a graph algorithm input file, and compile it.
```
    cd bin
    python ./graphitc.py -f ../../test/thundergp_input/pagerank_thundergp.gt -o ../../../ThunderGP/application/graphitpr/
```
4. set xilinx environment variable, and compile it into `xcl_bin` file.
```
    source /opt/xilinx/xrt/setup.sh
    source /home/tools/Xilinx/Vitis/2019.2/settings64.sh
    make app=graphitpr all
```
5. generate `host` file, load `dataset` and `xcl_bin` file 
```
    make app=graphitpr exe
    ./host_graph_fpga_graphitpr ./xclbin_graphitpr/graph_fpga.hw.xilinx_u280_xdma_201920_1.xclbin /Dataset/wiki-Talk.txt
```
