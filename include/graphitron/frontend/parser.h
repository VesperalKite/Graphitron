//
// Created by Zheng Feng on 11/16/22.
//

#ifndef GRAPHITRON_PARSER_H
#define GRAPHITRON_PARSER_H

#include <graphitron/frontend/fir.h>
#include <graphitron/frontend/token.h>
#include <graphitron/utils/scopedmap.h>

namespace graphitron{
    class Parser{
    public:
        Parser(std::vector<ParseError> *errors);
        fir::Program::Ptr parse(const TokenStream &);

    private:
        class SyntaxError : public std::exception{
        };

        enum class IdentType{
            FUNCTION, OTHER
        };

        typedef util::ScopedMap<std::string, IdentType> SymbolTable;

    private:
        fir::Program::Ptr parseProgram();

        fir::FIRNode::Ptr parseProgramElement();

        fir::ElementTypeDecl::Ptr parseElementTypeDecl();

        fir::Identifier::Ptr parseIdent();

        fir::ConstDecl::Ptr parseConstDecl();

        fir::TensorType::Ptr parseTensorType();
    
        fir::ScalarType::Ptr parseScalarType();

        fir::ScalarType::Ptr parseTensorComponentType();

        fir::NDTensorType::Ptr parseVectorBlockType();

        // std::vector<fir::FieldDecl::Ptr> parseFieldDeclList();

        // fir::FieldDecl::Ptr parseFieldDecl();

        // fir::FIRNode::Ptr parseExternFuncOrDecl();

        // fir::ExternDecl::Ptr parseExternDecl();

        // fir::FuncDecl::Ptr parseExternFuncDecl();

        fir::FuncDecl::Ptr parseFuncDecl();

        // std::vector<fir::GenericParam::Ptr> parseGenericParams();

        // fir::GenericParam::Ptr parseGenericParam();

        // std::vector<fir::Argument::Ptr> parseFunctorArgs();

        fir::FuncExpr::Ptr parseFunctorExpr();

        std::vector<fir::Argument::Ptr> parseArguments();

        fir::Argument::Ptr parseArgumentDecl();

        std::vector<fir::IdentDecl::Ptr> parseResults();

        fir::StmtBlock::Ptr parseStmtBlock();

        fir::Stmt::Ptr parseStmt();

        fir::VarDecl::Ptr parseVarDecl();

        fir::IdentDecl::Ptr parseIdentDecl();

        // fir::IdentDecl::Ptr parseTensorDecl();

        fir::WhileStmt::Ptr parseWhileStmt();

        fir::DoWhileStmt::Ptr parseDoWhileStmt();

        fir::IfStmt::Ptr parseIfStmt();

        fir::Stmt::Ptr parseElseClause();

        fir::ForStmt::Ptr parseForStmt();

        // fir::ParForStmt::Ptr parseParForStmt();

        fir::ForDomain::Ptr parseForDomain();

        fir::PrintStmt::Ptr parsePrintStmt();

        // fir::ExprStmt::Ptr parseDeleteStmt();

        // fir::ApplyStmt::Ptr parseApplyStmt();

        fir::ExprStmt::Ptr parseExprOrAssignStmt();

        fir::Expr::Ptr parseExpr();

        // fir::MapExpr::Ptr parseMapExpr();

        fir::Expr::Ptr parseOrExpr();

        fir::Expr::Ptr parseAndExpr();

        fir::Expr::Ptr parseBitOrExpr();

        fir::Expr::Ptr parseXorExpr();

        fir::Expr::Ptr parseBitAndExpr();

        fir::Expr::Ptr parseEqExpr();

        fir::Expr::Ptr parseTerm();

        fir::Expr::Ptr parseShiftExpr();

        fir::Expr::Ptr parseAddExpr();

        fir::Expr::Ptr parseMulExpr();

        fir::Expr::Ptr parseNegExpr();

        fir::Expr::Ptr parseTensorReadExpr();

        fir::Expr::Ptr parseFieldReadExpr();

        // fir::Expr::Ptr parseSetReadExpr();

        fir::Expr::Ptr parseFactor();

        fir::VarExpr::Ptr parseVarExpr();

        // fir::RangeConst::Ptr parseRangeConst();

        fir::CallExpr::Ptr parseCallExpr();

        // fir::UnnamedTupleReadExpr::Ptr parseUnnamedTupleReadExpr();

        // fir::NamedTupleReadExpr::Ptr parseNamedTupleReadExpr();

        // fir::Identifier::Ptr parseIdent();

        std::vector<fir::ReadParam::Ptr> parseReadParams();

        fir::ReadParam::Ptr parseReadParam();

        std::vector<fir::Expr::Ptr> parseExprParams();

        fir::Type::Ptr parseType();

        fir::ElementType::Ptr parseElementType();

        // fir::SetType::Ptr parseUnstructuredSetType();

        // fir::SetType::Ptr parseGridSetType();

        // std::vector<fir::Endpoint::Ptr> parseEndpoints();

        // fir::Endpoint::Ptr parseEndpoint();

        // fir::TupleElement::Ptr parseTupleElement();

        // fir::TupleType::Ptr parseNamedTupleType();

        // fir::TupleLength::Ptr parseTupleLength();

        // fir::TupleType::Ptr parseUnnamedTupleType();

        // fir::NDTensorType::Ptr parseMatrixBlockType();

        // fir::NDTensorType::Ptr parseTensorBlockType();

        // std::vector<fir::IndexSet::Ptr> parseIndexSets();

        // fir::IndexSet::Ptr parseIndexSet();

        // fir::SetIndexSet::Ptr parseSetIndexSet();

        fir::Expr::Ptr parseTensorLiteral();

        // fir::DenseTensorLiteral::Ptr parseDenseTensorLiteral();

        // fir::DenseTensorLiteral::Ptr parseDenseTensorLiteralInner();

        // fir::DenseTensorLiteral::Ptr parseDenseMatrixLiteral();

        // fir::DenseTensorLiteral::Ptr parseDenseVectorLiteral();

        // fir::IntVectorLiteral::Ptr parseDenseIntVectorLiteral();

        // fir::FloatVectorLiteral::Ptr parseDenseFloatVectorLiteral();

        // int parseSignedIntLiteral();

        // double parseSignedFloatLiteral();

        // fir::Test::Ptr parseTest();

        // //Graphit Set system
        fir::VertexSetType::Ptr parseVertexSetType();

        // fir::ListType::Ptr parseListType();

        fir::Type::Ptr parseEdgeSetType();

        // fir::NewExpr::Ptr parseNewExpr();

        fir::LoadExpr::Ptr parseLoadExpr();

        // fir::IntersectionExpr::Ptr parseIntersectionExpr();
        
        // fir::IntersectNeighborExpr::Ptr parseIntersectNeighborExpr();

        fir::ConstantVectorExpr::Ptr parseConstantVectorExpr();

        // // OG Additions
        // fir::PriorityQueueType::Ptr parsePriorityQueueType();


        void reportError(const Token &, std::string);

        Token peek(unsigned k = 0) const { return tokens.peek(k); }

        int findFirstOccurence(Token::Type type) const { return tokens.findFirstOccurence(type); }

        void skipTo(std::vector<Token::Type>);

        Token consume(Token::Type);

        bool tryConsume(Token::Type type) { return tokens.consume(type); }

        bool isIntrinsic(std::string func_name) {
            return std::find(intrinsics_.begin(), intrinsics_.end(), func_name) != intrinsics_.end();
        }
    private:
        SymbolTable decls;
        TokenStream tokens;
        std::vector<std::string> intrinsics_;
        std::vector<ParseError> *errors;

        void initIntrinsics();

        fir::BreakStmt::Ptr parseBreakStmt();
        fir::ReduceStmt::Ptr parseReduceStmt(Token::Type token_type, fir::ReduceStmt::ReductionOp reduce_op, fir::Expr::Ptr expr);
    };
}

#endif //GRAPHITRON_PARSER_H