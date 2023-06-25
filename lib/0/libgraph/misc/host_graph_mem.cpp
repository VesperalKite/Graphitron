#include "host_graph_sw.h"
#include "he_mem_config.h"


// HBM Banks requirements
#define MAX_HBM_BANKCOUNT 32
#define BANK_NAME(n) n | XCL_MEM_TOPOLOGY
const int bank[MAX_HBM_BANKCOUNT] = {
    BANK_NAME(0),  BANK_NAME(1),  BANK_NAME(2),  BANK_NAME(3),  BANK_NAME(4),
    BANK_NAME(5),  BANK_NAME(6),  BANK_NAME(7),  BANK_NAME(8),  BANK_NAME(9),
    BANK_NAME(10), BANK_NAME(11), BANK_NAME(12), BANK_NAME(13), BANK_NAME(14),
    BANK_NAME(15), BANK_NAME(16), BANK_NAME(17), BANK_NAME(18), BANK_NAME(19),
    BANK_NAME(20), BANK_NAME(21), BANK_NAME(22), BANK_NAME(23), BANK_NAME(24),
    BANK_NAME(25), BANK_NAME(26), BANK_NAME(27), BANK_NAME(28), BANK_NAME(29),
    BANK_NAME(30), BANK_NAME(31)};

void base_mem_init(cl_context &context)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(local_mem); i++)
    {
        he_mem_init(context, &local_mem[i]);
    }
}

void user_mem_init(cl_context &context)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(user_mem); i++)
    {
        user_mem[i].ext_attr.flags = bank[GetUserBankId(user_mem[i].id)];
        he_mem_init(context, &user_mem[i]);
    }
}

void partition_mem_init(cl_context &context, int blkIndex, int size)
{
    partitionDescriptor *partition = getPartition(blkIndex);

    partition->partSrc.id = MEM_ID_PARTITION_BASE + blkIndex * MEM_ID_PARTITION_OFFSET;
    partition->partSrc.name = "partition edge src";
    partition->partSrc.attr = ATTR_PL_HBM;
    partition->partSrc.unit_size = size * sizeof(int);
    partition->partSrc.size_attr = SIZE_DEFAULT;
    partition->partSrc.ext_attr.flags = bank[0];
    he_mem_init(context, &partition->partSrc);

    partition->partDst.id = MEM_ID_PARTITION_BASE + blkIndex * MEM_ID_PARTITION_OFFSET + 1;
    partition->partDst.name = "partition edge dst";
    partition->partDst.attr = ATTR_PL_HBM;
    partition->partDst.unit_size = size * sizeof(int);
    partition->partDst.size_attr = SIZE_DEFAULT;
    partition->partDst.ext_attr.flags = bank[1];
    he_mem_init(context, &partition->partDst);
    partition->parteProp.id = MEM_ID_PARTITION_BASE + blkIndex * MEM_ID_PARTITION_OFFSET + 2;
    partition->parteProp.name = "partition edge prop";
    partition->parteProp.attr = ATTR_PL_HBM;
    partition->parteProp.unit_size = size * sizeof(int);
    partition->parteProp.size_attr = SIZE_DEFAULT;
    partition->parteProp.ext_attr.flags = bank[5];
#if HAVE_EDGE_PROP
    he_mem_init(context, &partition->parteProp);
#endif
}