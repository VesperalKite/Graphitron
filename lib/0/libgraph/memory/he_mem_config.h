#ifndef __MEM_CONFIG_H__
#define __MEM_CONFIG_H__

#include "he_mem.h"


he_mem_t local_mem[] = {
    {
        MEM_ID_RPA,
        "rpa",
        ATTR_HOST_ONLY,
        sizeof(int) * 2,
        SIZE_IN_VERTEX,
    },
    {
        MEM_ID_CIA,
        "cia",
        ATTR_HOST_ONLY,
        sizeof(int) * 2,
        SIZE_IN_EDGE,
    },
    {
        MEM_ID_OUT_DEG,
        "outDeg",
        ATTR_PL_HBM,
        sizeof(int),
        SIZE_IN_VERTEX,
    },
    {
        MEM_ID_OUT_DEG_ORIGIN,
        "outDeg origin",
        ATTR_HOST_ONLY,
        sizeof(int),
        SIZE_IN_VERTEX,
    },
    {
        MEM_ID_VERTEX_INDEX_MAP,
        "indexMap",
        ATTR_HOST_ONLY,
        sizeof(int),
        SIZE_IN_VERTEX,
    }, 
    {
        MEM_ID_VERTEX_INDEX_REMAP,
        "indexRemap",
        ATTR_HOST_ONLY,
        sizeof(int),
        SIZE_IN_VERTEX,
    },
    {
        MEM_ID_EDGE_SRC,
        "edge src",
        ATTR_HOST_ONLY,
        sizeof(prop_t),
        SIZE_IN_EDGE,
    },
    {
        MEM_ID_EDGE_DST,
        "edge dst",
        ATTR_HOST_ONLY,
        sizeof(prop_t),
        SIZE_IN_EDGE,
    }
};

he_mem_t user_mem[]={
    // insert
};

#endif