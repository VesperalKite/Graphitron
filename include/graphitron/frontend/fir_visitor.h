//
// Created by Zheng Feng on 11/16/22.
//

#ifndef GRAPHITRON_FIR_VISITOR_H
#define GRAPHITRON_FIR_VISITOR_H

#include <memory>
#include <graphitron/midend/label_scope.h>

namespace graphitron {
    namespace fir {
        struct Program;
        struct StmtBlock; 
        struct SetIndexSet;
        struct ElementType;
        struct ScalarType;
        struct NDTensorType;
        struct Identifier;
        struct IdentDecl;
        struct ElementTypeDecl;
        struct Argument;
        struct FuncDecl;

        struct VarDecl;
        struct ConstDecl;
        struct WhileStmt;
        struct DoWhileStmt;
        struct IfStmt;
        struct RangeDomain;
        struct ForStmt;
        struct PrintStmt;
        struct ExprStmt;
        struct AssignStmt;

        struct Slice;
        struct ExprParam;
        struct UnaryExpr;
        struct BinaryExpr;
        struct NaryExpr;
        struct OrExpr;
        struct AndExpr;
        struct BitOrExpr;
        struct XorExpr;
        struct BitAndExpr;
        struct EqExpr;
        struct NotExpr;
        struct RshiftExpr;
        struct LshiftExpr;
        struct AddExpr;
        struct SubExpr;
        struct MulExpr;
        struct DivExpr;
        struct NegExpr;
        struct CallExpr;
        struct TensorReadExpr;
        struct FieldReadExpr;
        struct ParenExpr;
        struct VarExpr;
        struct IntLiteral;
        struct FloatLiteral;
        struct BoolLiteral;
        struct StringLiteral;
        struct VertexSetType;
        struct EdgeSetType;
        struct ConstantVectorExpr;
        struct EdgeSetLoadExpr;
        struct FuncExpr;
        struct MethodCallExpr;
        struct IterExpr;
        struct GsExpr;
        struct GsActiveExpr;
        struct ApplyExpr;
        struct InitExpr;
        struct ProcExpr;

        struct BreakStmt;
        struct ReduceStmt;

        struct FIRVisitor {
            virtual void visit(std::shared_ptr<Program>);

            virtual void visit(std::shared_ptr<StmtBlock>);

            //virtual void visit(std::shared_ptr<RangeIndexSet> op) {}

            virtual void visit(std::shared_ptr<SetIndexSet> op) {}

            //virtual void visit(std::shared_ptr<GenericIndexSet>);

            //virtual void visit(std::shared_ptr<DynamicIndexSet> op) {}

            virtual void visit(std::shared_ptr<ElementType> op) {}

            //virtual void visit(std::shared_ptr<Endpoint>);

            //virtual void visit(std::shared_ptr<HomogeneousEdgeSetType>);

            //virtual void visit(std::shared_ptr<HeterogeneousEdgeSetType>);

            //virtual void visit(std::shared_ptr<GridSetType>);

            //virtual void visit(std::shared_ptr<TupleElement>);

            //virtual void visit(std::shared_ptr<NamedTupleType>);

            //virtual void visit(std::shared_ptr<TupleLength> op) {}

            //virtual void visit(std::shared_ptr<UnnamedTupleType>);

            virtual void visit(std::shared_ptr<ScalarType> op) {}

            virtual void visit(std::shared_ptr<NDTensorType>);

            //virtual void visit(std::shared_ptr<ListType>);

            //virtual void visit(std::shared_ptr<OpaqueType>) {}

            virtual void visit(std::shared_ptr<Identifier> op) {}

            virtual void visit(std::shared_ptr<IdentDecl>);

            //virtual void visit(std::shared_ptr<FieldDecl>);

            virtual void visit(std::shared_ptr<ElementTypeDecl>);

            virtual void visit(std::shared_ptr<Argument>);

            //virtual void visit(std::shared_ptr<InOutArgument>);

            //virtual void visit(std::shared_ptr<ExternDecl>);

            //virtual void visit(std::shared_ptr<GenericParam> op) {}

            virtual void visit(std::shared_ptr<FuncDecl>);

            virtual void visit(std::shared_ptr<VarDecl>);

            virtual void visit(std::shared_ptr<ConstDecl>);

            virtual void visit(std::shared_ptr<WhileStmt>);

            virtual void visit(std::shared_ptr<DoWhileStmt>);

            virtual void visit(std::shared_ptr<IfStmt>);

            //virtual void visit(std::shared_ptr<IndexSetDomain>);

            virtual void visit(std::shared_ptr<RangeDomain>);

            virtual void visit(std::shared_ptr<ForStmt>);

            //virtual void visit(std::shared_ptr<ParForStmt>);

            //virtual void visit(std::shared_ptr<NameNode>);

            virtual void visit(std::shared_ptr<PrintStmt>);

            virtual void visit(std::shared_ptr<BreakStmt>) {}

            virtual void visit(std::shared_ptr<ExprStmt>);

            virtual void visit(std::shared_ptr<AssignStmt>);

            virtual void visit(std::shared_ptr<ReduceStmt>);

            virtual void visit(std::shared_ptr<Slice> op) {}

            virtual void visit(std::shared_ptr<ExprParam>);

            //virtual void visit(std::shared_ptr<MapExpr>);

            //virtual void visit(std::shared_ptr<ReducedMapExpr>);

            //virtual void visit(std::shared_ptr<UnreducedMapExpr>);

            virtual void visit(std::shared_ptr<OrExpr>);

            virtual void visit(std::shared_ptr<AndExpr>);

            virtual void visit(std::shared_ptr<BitOrExpr>);

            virtual void visit(std::shared_ptr<XorExpr>);

            virtual void visit(std::shared_ptr<BitAndExpr>);

            virtual void visit(std::shared_ptr<EqExpr>);

            virtual void visit(std::shared_ptr<NotExpr>);

            virtual void visit(std::shared_ptr<RshiftExpr>);

            virtual void visit(std::shared_ptr<LshiftExpr>);

            virtual void visit(std::shared_ptr<AddExpr>);

            virtual void visit(std::shared_ptr<SubExpr>);

            virtual void visit(std::shared_ptr<MulExpr>);

            virtual void visit(std::shared_ptr<DivExpr>);

            //virtual void visit(std::shared_ptr<LeftDivExpr>);

            //virtual void visit(std::shared_ptr<ElwiseMulExpr>);

            //virtual void visit(std::shared_ptr<ElwiseDivExpr>);

            virtual void visit(std::shared_ptr<NegExpr>);

            //virtual void visit(std::shared_ptr<ExpExpr>);

            //virtual void visit(std::shared_ptr<TransposeExpr>);

            virtual void visit(std::shared_ptr<CallExpr>);

            virtual void visit(std::shared_ptr<TensorReadExpr>);

            //virtual void visit(std::shared_ptr<SetReadExpr>);

            //virtual void visit(std::shared_ptr<NamedTupleReadExpr>);

            //virtual void visit(std::shared_ptr<UnnamedTupleReadExpr>);

            virtual void visit(std::shared_ptr<FieldReadExpr>);

            virtual void visit(std::shared_ptr<ParenExpr>);

            virtual void visit(std::shared_ptr<VarExpr> op) {}

            //virtual void visit(std::shared_ptr<RangeConst>);

            virtual void visit(std::shared_ptr<IntLiteral> op) {}

            virtual void visit(std::shared_ptr<FloatLiteral> op) {}

            virtual void visit(std::shared_ptr<BoolLiteral> op) {}

            virtual void visit(std::shared_ptr<StringLiteral> op) {}

            //virtual void visit(std::shared_ptr<IntVectorLiteral> op) {}

            //virtual void visit(std::shared_ptr<FloatVectorLiteral> op) {}

            //virtual void visit(std::shared_ptr<NDTensorLiteral>);

            //virtual void visit(std::shared_ptr<ApplyStmt>);

            //virtual void visit(std::shared_ptr<Test>);


            //Facet additions

            virtual void visit(std::shared_ptr<VertexSetType>){};
            virtual void visit(std::shared_ptr<EdgeSetType>){};
            //virtual void visit(std::shared_ptr<VertexSetAllocExpr>);

            //virtual void visit(std::shared_ptr<ListAllocExpr>);
            //virtual void visit(std::shared_ptr<VectorAllocExpr>);

            //virtual void visit(std::shared_ptr<IntersectionExpr>);
            //virtual void visit(std::shared_ptr<IntersectNeighborExpr>);
            virtual void visit(std::shared_ptr<ConstantVectorExpr>);
            virtual void visit(std::shared_ptr<EdgeSetLoadExpr>);

            virtual void visit(std::shared_ptr<FuncExpr>);
            virtual void visit(std::shared_ptr<MethodCallExpr>);
            virtual void visit(std::shared_ptr<IterExpr>);
            virtual void visit(std::shared_ptr<GsExpr>);
            virtual void visit(std::shared_ptr<ApplyExpr>);
            virtual void visit(std::shared_ptr<InitExpr>);
            virtual void visit(std::shared_ptr<ProcExpr>);
            //virtual void visit(std::shared_ptr<WhereExpr>);
            //virtual void visit(std::shared_ptr<FromExpr>);
            //virtual void visit(std::shared_ptr<ToExpr>);
	
            // OG Additions

	        //virtual void visit(std::shared_ptr<PriorityQueueType>){};
            //virtual void visit(std::shared_ptr<PriorityQueueAllocExpr>);


        protected:
            LabelScope label_scope_;


        private:
            void visitUnaryExpr(std::shared_ptr<UnaryExpr>);

            void visitBinaryExpr(std::shared_ptr<BinaryExpr>);

            void visitNaryExpr(std::shared_ptr<NaryExpr>);

        };
    }
}

#endif //GRAPHITRON_FIR_VISITOR_H