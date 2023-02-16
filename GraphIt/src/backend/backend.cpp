//
// Created by Yunming Zhang on 2/14/17.
//

#include <graphit/backend/backend.h>

namespace graphit{
    int Backend::emitCPP(std::ostream &oss, std::string module_name) {
        CodeGenCPP* codegen_cpp = new CodeGenCPP(oss, mir_context_, module_name);
        int flag = codegen_cpp->genCPP();
        delete codegen_cpp;
        return flag;
    }
    int Backend::emitapply_kernel_mk(std::ostream &oss)
    {
        oss << "" << std::endl;
        return 0;
    }
    int Backend::emitbuild_mk(std::ostream &oss,graphit::ThunderGP_Schedule::Partition partitionschedule)
    {
        std ::string partition;
        if (partitionschedule == graphit::ThunderGP_Schedule::Partition::normal){
            partition = "normal";
        }else if (partitionschedule == graphit::ThunderGP_Schedule::Partition::secondOrderEstimator){
            partition = "secondOrderEstimator";
        }else{
            partition = "";
        }
        oss << "" << std::endl;
        oss << "#scatter-gather kernel" << std::endl;
        oss << "HAVE_EDGE_PROP=false" << std::endl;
        oss << "HAVE_UNSIGNED_PROP=false" << std::endl;
        oss << "" << std::endl;
        oss << "#apply kernel" << std::endl;
        oss << "HAVE_APPLY=true" << std::endl;
        oss << "CUSTOMIZE_APPLY=false" << std::endl;
        oss << "HAVE_APPLY_OUTDEG=true" << std::endl;
        oss << "" << std::endl;
        oss << "#scheduler" << std::endl;
        oss << "SCHEDULER="<<partition<< std::endl;
        oss << "" << std::endl;
        oss << "#entry" << std::endl;
        oss << "DEFAULT_ENTRY=true" << std::endl;
        return 0;
    }
    int Backend::emitconfig_mk(std::ostream &oss)
    {
        oss << "FREQ=280" << std::endl;
        oss << "QUEUE_SIZE_FILTER=16" << std::endl;
        oss << "QUEUE_SIZE_MEMORY=512" << std::endl;
        oss << "" << std::endl;
        oss << "LOG_SCATTER_CACHE_BURST_SIZE=6" << std::endl;
        oss << "" << std::endl;
        oss << "APPLY_REF_ARRAY_SIZE=1" << std::endl;
        return 0;
    }
    int Backend::emitdataPrepare_cpp(std::ostream &oss)
    {
        oss << "#include \"host_graph_api.h\""<< std::endl;
        oss << "#include \"fpga_application.h\""<< std::endl;
        oss << "" << std::endl;
        oss << "#define INT2FLOAT                   (pow(2,30))" << std::endl;
        oss << "" << std::endl;
        oss << "int float2int(float a) {" << std::endl;
        oss << "    return (int)(a * INT2FLOAT);" << std::endl;
        oss << "}" << std::endl;
        oss << "" << std::endl;
        oss << "float int2float(int a) {" << std::endl;
        oss << "    return ((float)a / INT2FLOAT);" << std::endl;
        oss << "}" << std::endl;
        oss << "" << std::endl;
        oss << "unsigned int dataPrepareGetArg(graphInfo *info)" << std::endl;
        oss << "{" << std::endl;
        oss << "    return float2int((1.0f - kDamp) / info->vertexNum);" << std::endl;
        oss << "}" << std::endl;
        oss << "" << std::endl;
        oss << "int dataPrepareProperty(graphInfo *info)" << std::endl;
        oss << "{" << std::endl;
        oss << "    int *tempPropValue   =  (int*)get_host_mem_pointer(MEM_ID_PROP_FOR_DATAPREPARE);" << std::endl;
        oss << "    int *outDeg          =  (int*)get_host_mem_pointer(MEM_ID_OUT_DEG_ORIGIN);" << std::endl;
        oss << "    prop_t *vertexPushinProp = (prop_t*)get_host_mem_pointer(MEM_ID_PUSHIN_PROP);" << std::endl;
        oss << "" << std::endl;
        oss << "    int vertexNum = info->vertexNum;" << std::endl;
        oss << "    int alignedVertexNum = get_he_mem(MEM_ID_PUSHIN_PROP)->size/sizeof(int);" << std::endl;
        oss << "" << std::endl;
        oss << "    float init_score_float = 1.0f / vertexNum;" << std::endl;
        oss << "    int init_score_int = float2int(init_score_float);" << std::endl;
        oss << "" << std::endl;
        oss << "" << std::endl;
        oss << "    for (int i = 0; i < vertexNum; i++) {" << std::endl;
        oss << "        tempPropValue[i] = init_score_int;" << std::endl;
        oss << "        if (outDeg[i] > 0)" << std::endl;
        oss << "        {" << std::endl;
        oss << "            vertexPushinProp[i] =  tempPropValue[i] / outDeg[i];" << std::endl;
        oss << "        }" << std::endl;
        oss << "        else" << std::endl;
        oss << "        {" << std::endl;
        oss << "            vertexPushinProp[i]  = 0;" << std::endl;
        oss << "        }" << std::endl;
        oss << "    }" << std::endl;
        oss << "" << std::endl;
        oss << "    for (int i = vertexNum; i < alignedVertexNum; i++) {" << std::endl;
        oss << "        vertexPushinProp[i]  = 0;" << std::endl;
        oss << "    }" << std::endl;
        oss << "    return 0;" << std::endl;
        oss << "}" << std::endl;
        return 0;
    }
    int Backend::emitmain_cpp(std::ostream &oss)
    {
        return 0;
    }
    int Backend::emitPython(std::ostream &oss, std::string module_name, std::string module_path) {
	CodeGenPython *codegen_python = new CodeGenPython(oss, mir_context_, module_name, module_path);
	int flag = codegen_python->genPython();
	delete codegen_python;
	return flag;
    }
}
