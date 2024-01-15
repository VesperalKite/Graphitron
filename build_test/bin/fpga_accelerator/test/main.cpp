#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include "intrinsics.h"
using namespace std;
graphInfo edges;
int * label;
int * labelcount;
void vp_tryvisit_fpga_kernel() {
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
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_LABEL));
        he_set_dirty(MEM_ID_LABEL);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_LABELCOUNT));
        he_set_dirty(MEM_ID_LABELCOUNT);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partVertexNum);
        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partDstIdStart);
        clEnqueueTask(acc->vpOps[0], Handler->kernel, 0, NULL, &partition->vpEvent[0]);
    }
    clFinish(acc->vpOps[0]);
}
struct reset
{
  void operator() (int v) 
  {
    label[v] = v;
  };
};
int main(int argc, char **argv) {
  graphAccelerator * acc = getAccelerator();
  acceleratorInit("graph_fpga");
  acceleratorDataLoad(argv[(1)], "normal", &edges);
  label = (int *) get_host_mem_pointer(MEM_ID_LABEL);
  labelcount = (int *) get_host_mem_pointer(MEM_ID_LABELCOUNT);
  loop_for_lambda((int)0, ALIGNVERTEX((int)builtin_getVertices(edges)), [&] (int init_iter) {
    reset()(init_iter);
  });
  builtin_migrate(edges, (bool) 1);
  startTimer() ;
  vp_tryvisit_fpga_kernel();
  double elapsed_time = stopTimer() ;
  DEBUG_PRINTF("%s", "elapsed time: ");
  DEBUG_PRINTF("%lf\n", elapsed_time);
  acceleratorDeinit();
  return 0;
}
