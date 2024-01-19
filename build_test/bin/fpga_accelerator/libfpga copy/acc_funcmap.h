//
// Created by Zheng Feng on 11/21/23.
//

#ifndef GRAPHITRON_ACC_FUNCMAP_H
#define GRAPHITRON_ACC_FUNCMAP_H

#include "acc_types.h"

inline prop_t gatherFunc(prop_t ori, prop_t update)
{
    if (ori == 0) {
        return update;
    } else {
        return MIN(ori, update);
    }
}

#ifndef IS_ACTIVE_VERTEX
#define IS_ACTIVE_VERTEX(a)			(1)
#endif

// /* source vertex property process */
// #define PROP_COMPUTE_STAGE0(srcProp)                preprocessProperty(srcProp)

// /* source vertex property & edge property */
// #define PROP_COMPUTE_STAGE1(srcProp, edgeProp)      scatterFunc(srcProp,edgeProp)

/* destination property update in RAW solver */
#define PROP_COMPUTE_STAGE2(ori, update)            gatherFunc(ori ,update)

/* destination property update dst buffer update */
#define PROP_COMPUTE_STAGE3(ori,update)             gatherFunc(ori, update)

/* destination property merge */
#define PROP_COMPUTE_STAGE4(ori,update)             gatherFunc(ori, update)

#endif //GRAPHITRON_ACC_FUNCMAP_H