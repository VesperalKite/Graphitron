#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include "intrinsics.h"
using namespace std;
graphInfo edges;
int * old_rank;
int * new_rank;
int * out_degree;
int * contrib;
int * error;
double damp;
int dampFixPoint;
int init_score;
int beta_score;
void ep_computeContrib_fpga_kernel() {
    graphAccelerator* acc = getAccelerator();
    int blkNum = edges.blkNum;
    for (int i = 0; i < blkNum; i++) {
        partitionDescriptor* partition = getPartition(i);
        epDescriptor* Handler = getEdgesProc(0);
        int argvi = 0;
        int partEdgeNum = partition->partEdgeNum;
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(partition->partSrc.id));
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(partition->partDst.id));
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_OLD_RANK));
        he_set_dirty(MEM_ID_OLD_RANK);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_NEW_RANK));
        he_set_dirty(MEM_ID_NEW_RANK);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_CONTRIB));
        he_set_dirty(MEM_ID_CONTRIB);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_ERROR));
        he_set_dirty(MEM_ID_ERROR);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(double ), &damp);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &dampFixPoint);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &init_score);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &beta_score);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partEdgeNum);
        clEnqueueTask(acc->epOps[0], Handler->kernel, 0, NULL, &partition->epEvent[0]);
    }
    clFinish(acc->epOps[0]);
}
void vp_computeError_fpga_kernel() {
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
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_OLD_RANK));
        he_set_dirty(MEM_ID_OLD_RANK);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_NEW_RANK));
        he_set_dirty(MEM_ID_NEW_RANK);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_CONTRIB));
        he_set_dirty(MEM_ID_CONTRIB);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_ERROR));
        he_set_dirty(MEM_ID_ERROR);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(double ), &damp);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &dampFixPoint);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &init_score);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &beta_score);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partVertexNum);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partDstIdStart);
        clEnqueueTask(acc->vpOps[0], Handler->kernel, 0, NULL, &partition->vpEvent[0]);
    }
    clFinish(acc->vpOps[0]);
}
void vp_updateVertex_fpga_kernel() {
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
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_OLD_RANK));
        he_set_dirty(MEM_ID_OLD_RANK);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_NEW_RANK));
        he_set_dirty(MEM_ID_NEW_RANK);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_CONTRIB));
        he_set_dirty(MEM_ID_CONTRIB);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_ERROR));
        he_set_dirty(MEM_ID_ERROR);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(double ), &damp);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &dampFixPoint);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &init_score);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int ), &beta_score);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partVertexNum);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partDstIdStart);
        clEnqueueTask(acc->vpOps[1], Handler->kernel, 0, NULL, &partition->vpEvent[1]);
    }
    clFinish(acc->vpOps[1]);
}
struct float2int
{
  int operator() (double x) 
  {
    int y ;
    y = (x * pow((2), (30)) );
    return y;
  };
};
struct reset
{
  void operator() (int v) 
  {
    old_rank[v] = init_score;
    new_rank[v] = (0);
  };
};
struct errorSum
{
  int operator() () 
  {
    int sum ;
    for ( int v = (0); v < builtin_getVertices(edges); v++ )
    {
      sum += error[v];
    }
    return sum;
  };
};
int main(int argc, char **argv) {
  graphAccelerator * acc = getAccelerator();
  acceleratorInit("graph_fpga");
  acceleratorDataLoad(argv[(1)], "normal", &edges);
  old_rank = (int *) get_host_mem_pointer(MEM_ID_OLD_RANK);
  new_rank = (int *) get_host_mem_pointer(MEM_ID_NEW_RANK);
  out_degree = (int *) get_host_mem_pointer(MEM_ID_OUT_DEG);
  contrib = (int *) get_host_mem_pointer(MEM_ID_CONTRIB);
  error = (int *) get_host_mem_pointer(MEM_ID_ERROR);
  damp = ((float) 0.85);
  dampFixPoint = (128);
  beta_score = float2int()(((((float) 1) - damp) / builtin_getVertices(edges)));
  init_score = float2int()((((float) 1) / builtin_getVertices(edges)));
  loop_for_lambda((int)0, ALIGNVERTEX((int)builtin_getVertices(edges)), [&] (int init_iter) {
    reset()(init_iter);
  });
  builtin_migrate(edges, (bool) 1);
  int sum ;
  for ( int trail = (0); trail < (10); trail++ )
  {
    startTimer() ;
    ep_computeContrib_fpga_kernel();
    vp_computeError_fpga_kernel();
    vp_updateVertex_fpga_kernel();
    double elapsed_time = stopTimer() ;
    transfer_data_from_pl(acc->context, acc->device, MEM_ID_ERROR);
    sum = errorSum()();
    DEBUG_PRINTF("%s", "iteration: ");
    DEBUG_PRINTF("%d\n", trail);
    DEBUG_PRINTF("%s", "errorSum: ");
    DEBUG_PRINTF("%d\n", sum);
    DEBUG_PRINTF("%s", "elapsed time: ");
    DEBUG_PRINTF("%lf\n", elapsed_time);
    int test ;
    test = (( test ) < ((1) ) ? test : (1));
  }
  acceleratorDeinit();
  return 0;
}
