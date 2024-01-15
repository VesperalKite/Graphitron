//
// Created by Zheng Feng on 11/16/23.
//

#ifndef GRAPHITRON_ACC_PARAMETERS_H
#define GRAPHITRON_ACC_PARAMETERS_H

#include <climits>
#include <math.h>
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                             */
/*                             AccKernel                                       */
/*                                                                             */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define AP_INT_MAX_W        4096
#define PE_NUM              16
#define EDGE_NUM            8
#define LOG2_PE_NUM         4
#define LOG2_EDGE_NUM       3

#define HASH_MASK               (PE_NUM-1)

#define DATA_WIDTH              (512)
#define INT_WIDTH               (32)
#define INT_WIDTH_SHIFT         (5)

#define SIZE_BY_INT             (DATA_WIDTH/INT_WIDTH)
#define LOG2_SIZE_BY_INT        (4)                         //change manual
#define SIZE_BY_INT_MASK        (SIZE_BY_INT - 1)

#define BURST_READ_SIZE         (4)
#define LOG2_BURST_READ_SIZE    (2)                         //change manual


#define BURST_BUFFER_SIZE       (SIZE_BY_INT)
#define BURST_BUFFER_MASK       (BURST_BUFFER_SIZE  - 1)

#define BURST_ALL_BITS          (DATA_WIDTH)

// burstread - readbuffer
#define BURSTREAD_BUFFER_DEPTH              (128)
#define LOG2_BURSTREAD_BUFFER_DEPTH         (7)
#define ALIGN_SIZE                          (BURSTREAD_BUFFER_DEPTH * 16)
// writeback - writebuffer
#define WRITEBACK_BUFFER_DEPTH              (64)
#define LOG2_WRITEBACK_BUFFER_DEPTH         (6)

#define ENDFLAG                             (0xffffffff)

#define BITMAP_SLICE_SIZE                   (16)
#define BITMAP_SLICE_SHIFT                  (4)
#define EDGE_MAX        (2*1024*1024)//5610680////163840 // (1024*1024)
#define BRAM_BANK       16
#define LOG2_BRAM_BANK  4
#define PAD_TYPE        int16
#define PAD_WITH        16

#ifndef FLAG_SET
#define FLAG_SET                (1u)
#endif

#ifndef FLAG_RESET
#define FLAG_RESET              (0u)
#endif

#define uchar unsigned char

// log msg
#ifdef SW_DEBUG
#include "stdio.h"
#define DEBUG_PRINTF(fmt,...)   printf(fmt,##__VA_ARGS__); fflush(stdout);
#else
#define DEBUG_PRINTF(fmt,...)   ;
#endif

// Cache Parameters
#define URAM_DEPTH              (4096)
#define URAM_PER_EDGE           (4)
#define CACHE_SIZE              (URAM_DEPTH * URAM_PER_EDGE * 2)

#define CACHE_ADDRESS_MASK      (URAM_DEPTH * 8 - 1)

#define CACHE_UPDATE_BURST      (1<<LOG_CACHEUPDATEBURST)
#define LOG_CACHEUPDATEBURST    (LOG_SCATTER_CACHE_BURST_SIZE)

#define CA_WIDTH                (32)

#define DELAY_BUFFER       (512)
#define LOG_DELAY_BUFFER   (9)
#define POOL_SIZE          (4096)


#define BURST_WRITE_SIZE        (64)
#define LOG2_BURST_WRITE_SIZE   (6)
#define TOTAL_STEAM_READ_NUM    ((BURST_WRITE_SIZE * SIZE_BY_INT / 2)>> LOG2_PE_NUM)
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                             */
/*                              User Define                                    */
/*                                                                             */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define SRC_PROP_NUM    1

#if HAVE_UNSIGNED_PROP
typedef  unsigned int       prop_t;
#else
typedef  int                prop_t;
#endif

#ifdef TARGET_PARTITION_SIZE
#undef PARTITION_SIZE
#define PARTITION_SIZE  (TARGET_PARTITION_SIZE)
#endif
#define MAX_VERTICES_IN_ONE_PARTITION           (PARTITION_SIZE)

#define INVALID (INT_MAX)
#define IS_VALID(a)     ((a == INVALID)?0:1)
#define MIN(a,b) ((a > b)?b:a)
#define MAX(a,b) ((a > b)?a:b)
#define PLUS(a,b) (a+b)
#define SUB(a,b) (a-b)

#endif //GRAPHITRON_ACC_PARAMETERS_H