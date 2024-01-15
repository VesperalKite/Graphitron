#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include "intrinsics.h"
using namespace std;
graphInfo edges;
int * parent;
int * parent_tmp;
int * tuple;
int * activeVertex;
int level;
void ep_EdgeTravse_fpga_kernel() {
    graphAccelerator* acc = getAccelerator();
    int blkNum = edges.blkNum;
    for (int i = 0; i < blkNum; i++) {
        partitionDescriptor* partition = getPartition(i);
        epDescriptor* Handler = getEdgesProc(0);
        int argvi = 0;
        int partEdgeNum = partition->partEdgeNum;
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(partition->partSrc.id));
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(partition->partDst.id));
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_PARENT));
        he_set_dirty(MEM_ID_PARENT);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_PARENT_TMP));
        he_set_dirty(MEM_ID_PARENT_TMP);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_TUPLE));
        he_set_dirty(MEM_ID_TUPLE);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_ACTIVEVERTEX));
        he_set_dirty(MEM_ID_ACTIVEVERTEX);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &level);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partEdgeNum);
        clEnqueueTask(acc->epOps[0], Handler->kernel, 0, NULL, &partition->epEvent[0]);
    }
    clFinish(acc->epOps[0]);
}
void vp_VertexUpdate_fpga_kernel() {
    graphAccelerator* acc = getAccelerator();
    int blkNum = edges.blkNum;
    for (int i = 0; i < blkNum; i++) {
        partitionDescriptor* partition = getPartition(i);
        vpDescriptor* Handler = getVerticesProc(0);
        int argvi = 0;
        int partVertexNum = ((partition->dstEnd - partition->dstStart)/(ALIGN_SIZE) + 1)*(ALIGN_SIZE);
        int partDstIdStart = partition->dstStart;
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_OUT_DEG));
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_RPA));
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_CIA));
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_PARENT));
        he_set_dirty(MEM_ID_PARENT);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_PARENT_TMP));
        he_set_dirty(MEM_ID_PARENT_TMP);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_TUPLE));
        he_set_dirty(MEM_ID_TUPLE);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_ACTIVEVERTEX));
        he_set_dirty(MEM_ID_ACTIVEVERTEX);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &level);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partVertexNum);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partDstIdStart);
        clEnqueueTask(acc->vpOps[0], Handler->kernel, 0, NULL, &partition->vpEvent[0]);
    }
    clFinish(acc->vpOps[0]);
}
void vp_VertexApply_fpga_kernel() {
    graphAccelerator* acc = getAccelerator();
    int blkNum = edges.blkNum;
    for (int i = 0; i < blkNum; i++) {
        partitionDescriptor* partition = getPartition(i);
        vpDescriptor* Handler = getVerticesProc(1);
        int argvi = 0;
        int partVertexNum = ((partition->dstEnd - partition->dstStart)/(ALIGN_SIZE) + 1)*(ALIGN_SIZE);
        int partDstIdStart = partition->dstStart;
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_OUT_DEG));
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_RPA));
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_CIA));
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_PARENT));
        he_set_dirty(MEM_ID_PARENT);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_PARENT_TMP));
        he_set_dirty(MEM_ID_PARENT_TMP);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_TUPLE));
        he_set_dirty(MEM_ID_TUPLE);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_ACTIVEVERTEX));
        he_set_dirty(MEM_ID_ACTIVEVERTEX);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &level);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partVertexNum);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partDstIdStart);
        clEnqueueTask(acc->vpOps[1], Handler->kernel, 0, NULL, &partition->vpEvent[1]);
    }
    clFinish(acc->vpOps[1]);
}
struct reset
{
  void operator() (int v) 
  {
    parent[v] =  -(1);
    parent_tmp[v] =  -(1);
  };
};
int main(int argc, char **argv) {
  graphAccelerator * acc = getAccelerator();
  acceleratorInit("graph_fpga");
  acceleratorDataLoad(argv[(1)], "normal", &edges);
  parent = (int *) get_host_mem_pointer(MEM_ID_PARENT);
  parent_tmp = (int *) get_host_mem_pointer(MEM_ID_PARENT_TMP);
  tuple = (int *) get_host_mem_pointer(MEM_ID_TUPLE);
  activeVertex = (int *) get_host_mem_pointer(MEM_ID_ACTIVEVERTEX);
  level = (1);
  loop_for_lambda((int)0, ALIGNVERTEX((int)builtin_getVertices(edges)), [&] (int init_iter) {
    reset()(init_iter);
  });
  parent[(1)] = (1);
  parent_tmp[(1)] = (1);
  builtin_migrate(edges, (bool) 1);
  int frontier_size = (1);
  while ( frontier_size)
  {
    ep_EdgeTravse_fpga_kernel();
    vp_VertexUpdate_fpga_kernel();
    vp_VertexApply_fpga_kernel();
    transfer_data_from_pl(acc->context, acc->device, MEM_ID_ACTIVEVERTEX);
    frontier_size = activeVertex[(0)];
    activeVertex[(0)] = (0);
    transfer_data_to_pl(acc->context, acc->device, MEM_ID_ACTIVEVERTEX);
  }
  acceleratorDeinit();
  return 0;
}
