//
// Created by Zheng Feng on 04/23/23.
//

#include <graphitron/midend/processkernel_lower.h>
#include <graphitron/midend/parameter.h>

namespace graphitron {
    void ProcessKernelLower::lower(){
        auto lower_gas_expr = LowerProcExpr(parameter_, mir_context_);
        lower_gas_expr.rewrite();

        if (parameter_ != nullptr && parameter_->fpga_parameters != nullptr) {
            mir_context_->freq = parameter_->fpga_parameters->Frequency;
            mir_context_->TargetPartitionSize = parameter_->fpga_parameters->TargetPartitionSize;
            mir_context_->TargetBandWidth = parameter_->fpga_parameters->TargetBandWidth;
        }  else {
            FpgaParameter default_fpga_parameter;
            mir_context_->freq = default_fpga_parameter.Frequency;
        }
    }

    void ProcessKernelLower::LowerProcExpr::visit(mir::VertexSetProcExpr::Ptr vertexproc) {
        auto current_scope_name = label_scope_.getCurrentScope();
        vertexproc->scope_label_name = current_scope_name;
        std::string func_name = vertexproc->input_function->function_name->name;
        std::string vp_kernel_name = "vp_"+func_name+"_kernel";
        ProcessKernelSpace vp_kernel;
        vp_kernel.input_func = mir_context_->getFunction(func_name);
        if (parameter_ != nullptr && parameter_->kernel_parameters != nullptr) {
            auto kernel_parameter = parameter_->kernel_parameters->find(current_scope_name);
            if (kernel_parameter != parameter_->kernel_parameters->end()) {
                vp_kernel.parameter = kernel_parameter->second;       
            }
        }
        (mir_context_->vp_kernels)[vp_kernel_name] = vp_kernel;
    }
 
    void ProcessKernelLower::LowerProcExpr::visit(mir::EdgeSetProcExpr::Ptr edgeproc) {
        auto current_scope_name = label_scope_.getCurrentScope();
        edgeproc->scope_label_name = current_scope_name;
        std::string func_name = edgeproc->input_function->function_name->name;
        std::string ep_kernel_name = "ep_"+func_name+"_kernel";
        ProcessKernelSpace ep_kernel;
        mir::FuncDecl::Ptr ep_func = mir_context_->getFunction(func_name);
        ep_kernel.input_func = ep_func;
        assert(ep_func->args.size() == 2);
        mir::Var src = ep_func->args.front();
        mir::Var dst = ep_func->args.back();

        PropDetector* detector = new PropDetector(mir_context_); 
        ep_func->body->accept(detector);

        std::vector<mir::TensorReadExpr::Ptr> vectors = detector->getVectors();
        std::cout << vectors.size() << std::endl;
        for (auto vector : vectors) {
            if (mir::isa<mir::VarExpr>(vector->index)) {
                mir::VarExpr::Ptr index = mir::to<mir::VarExpr>(vector->index);
                if (index->var.getName() == src.getName()) {
                //std::cout << vector->getTargetNameStr() << " is src Prop" << std::endl;
                    auto srcProp = mir::to<mir::VarExpr>(vector->target);
                    ep_kernel.readProps.emplace(srcProp->var.getName());
                } else if (index->var.getName() == dst.getName()) {
                    auto dstProp = mir::to<mir::VarExpr>(vector->target);
                    ep_kernel.writeProps.emplace(dstProp->var.getName());
                }
            }
            //std::cout << vector->getTargetNameStr() << std::endl;
        }



        if (parameter_ != nullptr && parameter_->kernel_parameters != nullptr) {
            auto kernel_parameter = parameter_->kernel_parameters->find(current_scope_name);
            if (kernel_parameter != parameter_->kernel_parameters->end()) {
                ep_kernel.parameter = kernel_parameter->second;
            }
        }
        (mir_context_->ep_kernels)[ep_kernel_name] = ep_kernel;
    }

    void ProcessKernelLower::PropDetector::visit(mir::VarExpr::Ptr expr) {
        std::cout << expr->var.getName();
    }

    void ProcessKernelLower::PropDetector::visit(mir::TensorReadExpr::Ptr expr) {
        std::cout << expr->getTargetNameStr();
        std::cout << "[";
        expr->index->accept(this);
        std::cout << "]";
        vectors.push_back(expr);
    }
}