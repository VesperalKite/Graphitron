#ifndef __HE_MEM_ID_H__
#define __HE_MEM_ID_H__

#define SIZE_IN_EDGE            (0)
#define SIZE_IN_VERTEX          (1)
#define SIZE_DEFAULT            (2)

#define MEM_ID_RPA                  (1)
#define MEM_ID_CIA                  (2)
#define MEM_ID_OUT_DEG              (3)
#define MEM_ID_OUT_DEG_ORIGIN       (4)
#define MEM_ID_VERTEX_INDEX_MAP     (5)
#define MEM_ID_VERTEX_INDEX_REMAP   (6)
#define MEM_ID_EDGE_SRC             (7)
#define MEM_ID_EDGE_DST             (8)
#define MEM_ID_EDGE_PROP            (9)
#define MEM_ID_PART_EDGE_PROP       (10)

#define MEM_ID_PARTITION_BASE        (100)
#define MEM_ID_PARTITION_OFFSET      (128)


#define MEM_ID_USER_DEFINE_BASE     (16384)
#define BANK_ID_USER_DEFINE_BASE    (5)

// User Define Part
#define SetUserMemId(i)             (MEM_ID_USER_DEFINE_BASE+i)
#define GetUserBankId(mem_id)       (mem_id-MEM_ID_USER_DEFINE_BASE+BANK_ID_USER_DEFINE_BASE)

#define MEM_ID_PARENT     SetUserMemId(1)
#define MEM_ID_PARENT_TMP     SetUserMemId(2)
#define MEM_ID_PROP_TEST     SetUserMemId(3)
#define MEM_ID_ACTIVEVERTEX     SetUserMemId(4)
#define MEM_ID_SECONDPROP     SetUserMemId(5)
// insert

#endif
