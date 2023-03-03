/* DEFAULT_ENTRY in build.mk */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>

#include "host_graph_api.h"
#include "host_graph_verification.h"

#ifndef XCL_BIN
#define XCL_BIN NULL
#endif

using namespace std;

graphInfo graphDataInfo;

int main(int argc, char **argv) {

    char * xcl_file = NULL;
    std::string gName;
    if (argc <= 2){
        if (XCL_BIN == NULL){
            DEBUG_PRINTF("xcl_file is null, please check file exitst!");
            exit(-1);
        }
        xcl_file = XCL_BIN;
        if (argc > 1){
            gName = argv[1];
        }else{
            gName = "wiki-talk";
        }
    } else {
        DEBUG_PRINTF("Host_exe [graph_set].")
        exit(-1);
    }

    std::string mode = "normal";

    DEBUG_PRINTF("start main\n");

    acceleratorInit("graph_fpga", xcl_file);

    acceleratorDataLoad(gName, mode, &graphDataInfo);

    acceleratorDataPreprocess(&graphDataInfo);
    /* for verification */
    acceleratorCModelDataPreprocess(&graphDataInfo);

    for (int runCounter = 0 ; runCounter < 10 ; runCounter ++)
    {
        double startStamp, endStamp;
        startStamp = getCurrentTimestamp();

        acceleratorSuperStep(runCounter, &graphDataInfo);

        endStamp = getCurrentTimestamp();
        /* for verification */
        acceleratorCModelSuperStep(runCounter, &graphDataInfo);
 
        /* for profile */
        acceleratorProfile(runCounter, runCounter, &graphDataInfo, endStamp - startStamp);
    }
    acceleratorDeinit();

    return 0;
}

