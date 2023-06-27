# Graphitron
A powerful, high-performance language for graph processing
## Run the Code
#### Prerequisites
* The gcc-8.3 or above
* The CMake-3.5.0 or above
* Tools:
    * Vitis 2019.2
* Evaluated platforms from Xilinx:
    * Alveo U50  Data Center Accelerator Card
    * Alveo U280 Data Center Accelerator Card
#### Here is the example of implementing the accelerator for BFS on Alveo U280 platform with Vitis 2019.2
```sh
# build `graphitron compiler`.
$ cd ./Graphitron
$ mkdir build
$ cd build
$ cmake ..
$ make
# set xilinx environment variable
$ source .../setup.sh
$ source .../settings64.sh
# compile a application
$ make app=bfs all
# load `dataset` file, run application
$ ./host_graph_fpga_bfs $(Dataset)/wiki-Talk.txt