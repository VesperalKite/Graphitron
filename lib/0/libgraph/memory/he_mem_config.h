#ifndef __MEM_CONFIG_H__
#define __MEM_CONFIG_H__


he_mem_t local_mem[] = {
    {
        MEM_ID_RPA,
        "rpa",
        ATTR_HOST_ONLY,
        sizeof(int) * 2,
        SIZE_IN_VERTEX,
    },


};

#endif