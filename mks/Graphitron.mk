TARGETS := hw

APP := bfs
#defalut optimization: 
# 0: More freedom but no guarantee of performance
# 1: Using ECP-GAS Model to obtain high performance
opt := 0 

TARGET_BANDWIDTH := 77

DEVICES := xilinx_u280_xdma_201920_1

SUB_PARTITION_NUM := 1

PARTITION_SIZE := 524288

DEFAULT_DATASET_PATH := /home/fengzheng/Graphset/