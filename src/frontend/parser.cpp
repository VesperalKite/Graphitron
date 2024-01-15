//
// Created by Zheng Feng on 11/23/22.
//

#include <graphitron/frontend/parser.h>

namespace graphitron {

    Parser::Parser(std::vector<ParseError> *errors) : errors(errors) {}
    fir::Program::Ptr Parser::parse(const TokenStream &tokens) {
        this->tokens = tokens;

        decls = SymbolTable();

        initIntrinsics();

        return parseProgram();
    }

// program: {program_element}
    fir::Program::Ptr Parser::parseProgram() {
        auto program = std::make_shared<fir::Program>();

        while (peek().type != Token::Type::END) {
            const fir::FIRNode::Ptr element = parseProgramElement();
            if (element) {
                program->elems.push_back(element);
            }
        }

        return program;
    }

// program_element: element_type_decl | const_decl | func_decl
    fir::FIRNode::Ptr Parser::parseProgramElement() {
        try {
            switch (peek().type) {
                case Token::Type::ELEMENT:
                    return parseElementTypeDecl();
                case Token::Type::CONST:
                    return parseConstDecl();
                case Token::Type::FUNC:
                    return parseFuncDecl();
                default:
                    reportError(peek(), "a program element type");
                    throw SyntaxError();
                    break;
            }
        } catch (const SyntaxError &) {
            skipTo({Token::Type::FUNC, Token::Type::ELEMENT, Token::Type::CONST});

            return fir::FIRNode::Ptr();
        }
    }
//********************************************************************//
//************************** ElementDecl *****************************//
//********************************************************************//

// element_type_decl: 'element' ident 'end'
    fir::ElementTypeDecl::Ptr Parser::parseElementTypeDecl() {
        auto elemTypeDecl = std::make_shared<fir::ElementTypeDecl>();

        const Token elementToken = consume(Token::Type::ELEMENT);
        elemTypeDecl->setBeginLoc(elementToken);

        elemTypeDecl->name = parseIdent();

        const Token endToken = consume(Token::Type::BLOCKEND);
        elemTypeDecl->setEndLoc(endToken);

        return elemTypeDecl;
    }
// ident: IDENT
    fir::Identifier::Ptr Parser::parseIdent() {
        auto ident = std::make_shared<fir::Identifier>();

        const Token identToken = consume(Token::Type::IDENT);
        ident->setLoc(identToken);
        ident->ident = identToken.str;

        return ident;
    }
//********************************************************************//
//*************************** ConstDecl ******************************//
//********************************************************************//

// const_decl: 'const' ident [':' type] '=' expr ';' 
    fir::ConstDecl::Ptr Parser::parseConstDecl() {
        try {
            auto constDecl = std::make_shared<fir::ConstDecl>();

            const Token constToken = consume(Token::Type::CONST);
            constDecl->setBeginLoc(constToken);

            constDecl->name = parseIdent();
            if (tryConsume(Token::Type::COL)) {
                constDecl->type = parseType();
                //constDecl->type = parseTensorType();
            }

            //if an initial value is specified
            if (tryConsume(Token::Type::ASSIGN)) {
                //consume(Token::Type::ASSIGN);
                //{}
                if (peek().type == Token::Type::LC) {
                    constDecl->initVal = parseConstantVectorExpr();
                } else {
                    constDecl->initVal = parseExpr();
                }
            }

            const Token endToken = consume(Token::Type::SEMICOL);
            constDecl->setEndLoc(endToken);

            decls.insert(constDecl->name->ident, IdentType::OTHER);

            return constDecl;

        } catch (const SyntaxError &) {
            skipTo({Token::Type::SEMICOL});
            consume(Token::Type::SEMICOL);

            return fir::ConstDecl::Ptr();
        }
    }
// type: element_type | unstructured_set_type | grid_set_type
//     | tuple_type | tensor_type
    fir::Type::Ptr Parser::parseType() {
        fir::Type::Ptr type;
        switch (peek().type) {
            case Token::Type::IDENT:
                type = parseElementType();
                break;
            case Token::Type::EDGE_SET:
                type = parseEdgeSetType();
                break;
            case Token::Type::VERTEX_SET:
                type = parseVertexSetType();
                break;
            case Token::Type::DOUBLE:
            case Token::Type::INT:
            case Token::Type::INTX:
            case Token::Type::FLOAT:
            case Token::Type::BOOL:
            case Token::Type::STRING:
            case Token::Type::VECTOR:
                type = parseTensorType();
                break;
            default:
                reportError(peek(), "a type identifier");
                throw SyntaxError();
                break;
        }

        return type;
    }
    fir::Type::Ptr Parser::parseEdgeSetType() {

        auto edgeSetType = std::make_shared<fir::EdgeSetType>();

        const Token setToken = consume(Token::Type::EDGE_SET);
        consume(Token::Type::LC);
        const fir::ElementType::Ptr element = parseElementType();
        const Token rightCurlyToken = consume(Token::Type::RC);

        consume(Token::Type::LP);
        std::vector<fir::ElementType::Ptr> vertex_element_type_list;
        //do {

        // parse src element type
        const fir::ElementType::Ptr src_vertex_element_type = parseElementType();
        vertex_element_type_list.push_back(src_vertex_element_type);
        consume(Token::Type::COMMA);
        // parse dst element type
        const fir::ElementType::Ptr dst_vertex_element_type = parseElementType();
        vertex_element_type_list.push_back(dst_vertex_element_type);

        // if a third type argument is supplied, we assume that it is a weight
        if (tryConsume(Token::Type::COMMA)) {
            const fir::ScalarType::Ptr weight_type = parseScalarType();
            edgeSetType->weight_type = weight_type;
        }


        //} while (tryConsume(Token::Type::COMMA));

        const Token rightP = consume(Token::Type::RP);


        edgeSetType->setBeginLoc(setToken);
        edgeSetType->edge_element_type = element;
        edgeSetType->setEndLoc(rightCurlyToken);
        edgeSetType->vertex_element_type_list = vertex_element_type_list;
        return edgeSetType;
    }
    //parse the vertexset type vertexset{ElementType}
    fir::VertexSetType::Ptr Parser::parseVertexSetType() {
        const Token setToken = consume(Token::Type::VERTEX_SET);
        consume(Token::Type::LC);
        const fir::ElementType::Ptr element = parseElementType();
        const Token rightCurlyToken = consume(Token::Type::RC);

        auto vertexSetType = std::make_shared<fir::VertexSetType>();
        vertexSetType->setBeginLoc(setToken);
        vertexSetType->element = element;
        vertexSetType->setEndLoc(rightCurlyToken);

        return vertexSetType;
    }
// tensor_type: scalar_type | vector_block_type [''']
    fir::TensorType::Ptr Parser::parseTensorType() {
        fir::NDTensorType::Ptr tensorType;
        switch (peek().type) {
            case Token::Type::INTX:
            case Token::Type::INT:
            case Token::Type::FLOAT:
            case Token::Type::DOUBLE:
            case Token::Type::BOOL:
            case Token::Type::STRING:
                return parseScalarType();
            default:
                tensorType = parseVectorBlockType();
                break;
        }

        return tensorType;
    }
// vector_block_type: va
    fir::NDTensorType::Ptr Parser::parseVectorBlockType() {
        auto tensorType = std::make_shared<fir::NDTensorType>();

        const Token tensorToken = consume(Token::Type::VECTOR);
        tensorType->setBeginLoc(tensorToken);
        //{}
        // added to support specifying the Element Type.
        // The vector is essentially a field of the Element
        if (tryConsume(Token::Type::LC)) {
            const fir::ElementType::Ptr element = parseElementType();
            tensorType->element = element;
            consume(Token::Type::RC);
        }
        //()
        consume(Token::Type::LP);
        if (peek().type == Token::Type::VECTOR) {
            tensorType->blockType = parseVectorBlockType();
        } else {
            tensorType->blockType = parseScalarType();
        }

        const Token rightParenToken = consume(Token::Type::RP);
        tensorType->setEndLoc(rightParenToken);

        return tensorType;
    }
// scalar_type: 'string' | tensor_component_type
    fir::ScalarType::Ptr Parser::parseScalarType() {
        if (peek().type == Token::Type::STRING) {
            auto stringType = std::make_shared<fir::ScalarType>();

            const Token stringToken = consume(Token::Type::STRING);
            stringType->setLoc(stringToken);
            stringType->type = fir::ScalarType::Type::STRING;

            return stringType;
        }

        return parseTensorComponentType();
    }
// tensor_component_type: 'intx' | 'int' | 'float' | 'bool' | 'double'
    fir::ScalarType::Ptr Parser::parseTensorComponentType() {
        auto scalarType = std::make_shared<fir::ScalarType>();

        const Token typeToken = peek();
        scalarType->setLoc(typeToken);

        switch (peek().type) {
            case Token::Type::INTX:
                consume(Token::Type::INTX);
                scalarType->type = fir::ScalarType::Type::INTX;
                break;
            case Token::Type::INT:
                consume(Token::Type::INT);
                scalarType->type = fir::ScalarType::Type::INT;
                break;
            case Token::Type::FLOAT:
                consume(Token::Type::FLOAT);
                scalarType->type = fir::ScalarType::Type::FLOAT;
                break;
            case Token::Type::DOUBLE:
                consume(Token::Type::DOUBLE);
                scalarType->type = fir::ScalarType::Type::DOUBLE;
                break;
            case Token::Type::BOOL:
                consume(Token::Type::BOOL);
                scalarType->type = fir::ScalarType::Type::BOOL;
                break;
            case Token::Type::STRING:
                // TODO: Implement.
            default:
                reportError(peek(), "a tensor component type identifier");
                throw SyntaxError();
                break;
        }

        return scalarType;
    }

// element_type: ident
    fir::ElementType::Ptr Parser::parseElementType() {
        auto elementType = std::make_shared<fir::ElementType>();

        const Token typeToken = consume(Token::Type::IDENT);
        elementType->setLoc(typeToken);
        elementType->ident = typeToken.str;

        return elementType;
    }
//{element ,element, ...}
    fir::ConstantVectorExpr::Ptr Parser::parseConstantVectorExpr() {
        const auto constVector = std::make_shared<fir::ConstantVectorExpr>();
        std::vector<fir::Expr::Ptr> elements;
        consume(Token::Type::LC);
        if (peek().type != Token::Type::RC) {
            do {
                const fir::Expr::Ptr element = parseExpr();
                elements.push_back(element);
            } while (tryConsume(Token::Type::COMMA));
        }
        consume(Token::Type::RC);

        constVector->vectorElements = elements;
        constVector->numElements = elements.size();

        return constVector;
    }

// expr: map_expr | new_expr | or_expr
    fir::Expr::Ptr Parser::parseExpr() {
        switch (peek().type) {
            case Token::Type::LOAD:
                return parseLoadExpr();
            default:
                return parseOrExpr();
        }
    }

    // load_expr: 'load' '(' expr ')'
    fir::LoadExpr::Ptr Parser::parseLoadExpr() {

        const Token newToken = consume(Token::Type::LOAD);
        const auto load_expr = std::make_shared<fir::EdgeSetLoadExpr>();

        consume(Token::Type::LP);
        const auto file_name = parseExpr();
        consume(Token::Type::RP);
        load_expr->file_name = file_name;
        return load_expr;
    }
// or_expr: and_expr {'or' and_expr}
    fir::Expr::Ptr Parser::parseOrExpr() {
        fir::Expr::Ptr expr = parseAndExpr();

        while (tryConsume(Token::Type::OR)) {
            auto orExpr = std::make_shared<fir::OrExpr>();

            orExpr->lhs = expr;
            orExpr->rhs = parseAndExpr();

            expr = orExpr;
        }

        return expr;
    }
// and_expr: '|' {'and' '|'}
    fir::Expr::Ptr Parser::parseAndExpr() {
        fir::Expr::Ptr expr = parseBitOrExpr();

        while (tryConsume(Token::Type::AND)) {
            auto andExpr = std::make_shared<fir::AndExpr>();

            andExpr->lhs = expr;
            andExpr->rhs = parseBitOrExpr();

            expr = andExpr;
        }

        return expr;
    }

// BitOr_expr: xor_expr {'|' xor_expr}
    fir::Expr::Ptr Parser::parseBitOrExpr() {
        fir::Expr::Ptr expr = parseXorExpr();

        while (tryConsume(Token::Type::BIT_OR)) {
            auto bitorExpr = std::make_shared<fir::BitOrExpr>();

            bitorExpr->lhs = expr;
            bitorExpr->rhs = parseXorExpr();

            expr = bitorExpr;
        }

        return expr;
    }

// xor_expr: '&' {'xor' '&'}
    fir::Expr::Ptr Parser::parseXorExpr() {
        fir::Expr::Ptr expr = parseBitAndExpr();

        while (tryConsume(Token::Type::XOR)) {
            auto xorExpr = std::make_shared<fir::XorExpr>();

            xorExpr->lhs = expr;
            xorExpr->rhs = parseBitAndExpr();

            expr = xorExpr;
        }

        return expr;
    }

// BitAnd_expr: eq_expr {'&' eq_expr}
    fir::Expr::Ptr Parser::parseBitAndExpr() {
        fir::Expr::Ptr expr = parseEqExpr();
        while (tryConsume(Token::Type::BIT_AND)) {
            auto bitandExpr = std::make_shared<fir::BitAndExpr>();

            bitandExpr->lhs = expr;
            bitandExpr->rhs = parseEqExpr();
            
            expr = bitandExpr;
        }

        return expr;
    }

// eq_expr: term {('==' | '!=' | '>' | '<' | '>=' | '<=') term}
    fir::Expr::Ptr Parser::parseEqExpr() {
        auto expr = std::make_shared<fir::EqExpr>();

        const fir::Expr::Ptr operand = parseTerm();
        expr->operands.push_back(operand);

        while (true) {
            switch (peek().type) {
                case Token::Type::EQ:
                    consume(Token::Type::EQ);
                    expr->ops.push_back(fir::EqExpr::Op::EQ);
                    break;
                case Token::Type::NE:
                    consume(Token::Type::NE);
                    expr->ops.push_back(fir::EqExpr::Op::NE);
                    break;
                case Token::Type::RA:
                    consume(Token::Type::RA);
                    expr->ops.push_back(fir::EqExpr::Op::GT);
                    break;
                case Token::Type::LA:
                    consume(Token::Type::LA);
                    expr->ops.push_back(fir::EqExpr::Op::LT);
                    break;
                case Token::Type::RE:
                    consume(Token::Type::RE);
                    expr->ops.push_back(fir::EqExpr::Op::GE);
                    break;
                case Token::Type::LE:
                    consume(Token::Type::LE);
                    expr->ops.push_back(fir::EqExpr::Op::LE);
                    break;
                default:
                    return (expr->operands.size() == 1) ? expr->operands[0] : expr;
            }

            const fir::Expr::Ptr operand = parseTerm();
            expr->operands.push_back(operand);
        }
    }

// term: ('not' term) | add_expr
    fir::Expr::Ptr Parser::parseTerm() {
        if (peek().type == Token::Type::NOT) {
            auto notExpr = std::make_shared<fir::NotExpr>();

            const Token notToken = consume(Token::Type::NOT);
            notExpr->setBeginLoc(notToken);
            notExpr->operand = parseTerm();

            return notExpr;
        }

        return parseShiftExpr();
    }

//shift_expr : add_expr {('>>' | '<<') add_expr}
    fir::Expr::Ptr Parser::parseShiftExpr() {
        fir::Expr::Ptr expr = parseAddExpr();

        while (true) {
            fir::BinaryExpr::Ptr shiftExpr;
            switch (peek().type) {
                case Token::Type::RS:  
                    consume(Token::Type::RS);
                    shiftExpr = std::make_shared<fir::RshiftExpr>();
                    break;
                case Token::Type::LS:  
                    consume(Token::Type::LS);
                    shiftExpr = std::make_shared<fir::LshiftExpr>();
                    break;
                default: 
                    return expr;
            }

            shiftExpr->lhs = expr;
            shiftExpr->rhs = parseAddExpr();

            expr = shiftExpr;
        } 
    }

// add_expr : mul_expr {('+' | '-') mul_expr}
    fir::Expr::Ptr Parser::parseAddExpr() {
        fir::Expr::Ptr expr = parseMulExpr();

        while (true) {
            fir::BinaryExpr::Ptr addExpr;
            switch (peek().type) {
                case Token::Type::PLUS:
                    consume(Token::Type::PLUS);
                    addExpr = std::make_shared<fir::AddExpr>();
                    break;
                case Token::Type::SUB:
                    consume(Token::Type::SUB);
                    addExpr = std::make_shared<fir::SubExpr>();
                    break;
                default:
                    return expr;
            }

            addExpr->lhs = expr;
            addExpr->rhs = parseMulExpr();

            expr = addExpr;
        }
    }

// mul_expr: neg_expr {('*' | '/' | '\' | '.*' | './') neg_expr}
    fir::Expr::Ptr Parser::parseMulExpr() {
        fir::Expr::Ptr expr = parseNegExpr();

        while (true) {
            fir::BinaryExpr::Ptr mulExpr;
            switch (peek().type) {
                case Token::Type::MUL:
                    consume(Token::Type::MUL);
                    mulExpr = std::make_shared<fir::MulExpr>();
                    break;
                case Token::Type::DIV:
                    consume(Token::Type::DIV);
                    mulExpr = std::make_shared<fir::DivExpr>();
                    break;
                default:
                    return expr;
            }

            mulExpr->lhs = expr;
            mulExpr->rhs = parseNegExpr();

            expr = mulExpr;
        }
    }

// neg_expr: (('+' | '-') neg_expr) | exp_expr
    fir::Expr::Ptr Parser::parseNegExpr() {
        auto negExpr = std::make_shared<fir::NegExpr>();

        switch (peek().type) {
            case Token::Type::SUB: {
                const Token minusToken = consume(Token::Type::SUB);
                negExpr->setBeginLoc(minusToken);
                negExpr->negate = true;
                break;
            }
            case Token::Type::PLUS: {
                const Token plusToken = consume(Token::Type::PLUS);
                negExpr->setBeginLoc(plusToken);
                negExpr->negate = false;
                break;
            }
            default:
                return parseTensorReadExpr();
        }
        negExpr->operand = parseNegExpr();

        return negExpr;
    }
// tensor_read_expr: field_read_expr {'[' [read_params] ']'}


    fir::Expr::Ptr Parser::parseTensorReadExpr() {
        fir::Expr::Ptr expr = parseFieldReadExpr();

        while (tryConsume(Token::Type::LB)) {
            auto tensorRead = std::make_shared<fir::TensorReadExpr>();
            tensorRead->tensor = expr;

            if (peek().type != Token::Type::RB) {
                tensorRead->indices = parseReadParams();
            }

            const Token rightParenToken = consume(Token::Type::RB);
            tensorRead->setEndLoc(rightParenToken);

            expr = tensorRead;
        }

        return expr;
    }

// field_read_expr: set_read_expr '.' {ident '(' [ expr_params ] ')' | iter '('ident')' | gs '(' ident, ident, [ident]')' | apply '(' ident ')'}
    fir::Expr::Ptr Parser::parseFieldReadExpr() {
        // We don't need to supprot set read expressions, so we just work with factors directly
        //fir::Expr::Ptr expr = parseSetReadExpr();
        fir::Expr::Ptr expr = parseFactor();

        fir::IterExpr::Ptr iter_expr;

        while (tryConsume(Token::Type::PERIOD)) {

            if (tryConsume(Token::Type::ITER)) {
                consume(Token::Type::LP);
                iter_expr = std::make_shared<fir::IterExpr>();
                iter_expr->args = parseExprParams();
                consume(Token::Type::RP);
            } else if (tryConsume(Token::Type::GS)) {
                consume(Token::Type::LP);
                auto gs_expr = std::make_shared<fir::GsExpr>();
                gs_expr->target = expr;
                std::vector<fir::FuncExpr::Ptr> input_funcs;
                do {
                    const fir::FuncExpr::Ptr input_function = parseFunctorExpr();
                    input_funcs.push_back(input_function);
                } while(tryConsume(Token::Type::COMMA));
                consume(Token::Type::RP);
                if (input_funcs.size() == 2) {
                    gs_expr->input_scatter_function = input_funcs[0];
                    gs_expr->input_gather_function = input_funcs[1];
                    gs_expr->input_active_function = nullptr;
                } else if (input_funcs.size() == 3) {
                    gs_expr->input_scatter_function = input_funcs[0];
                    gs_expr->input_active_function = input_funcs[1];
                    gs_expr->input_gather_function = input_funcs[2];
                } else {
                    reportError(peek(), "gs function format error!");
                }
                gs_expr->iter_expr = iter_expr;
                expr = gs_expr;
            } else if (tryConsume(Token::Type::SCATTER)) {
                consume(Token::Type::LP);
                auto gs_expr = std::make_shared<fir::GsExpr>();
                gs_expr->target = expr;
                gs_expr->input_scatter_function = parseFunctorExpr();
                consume(Token::Type::RP);
                consume(Token::Type::PERIOD);
                if (tryConsume(Token::Type::FILTER)) {
                    consume(Token::Type::LP);
                    gs_expr->input_active_function = parseFunctorExpr();
                    consume(Token::Type::RP);
                    consume(Token::Type::PERIOD);
                }
                consume(Token::Type::GATHER);
                consume(Token::Type::LP);
                gs_expr->input_gather_function = parseFunctorExpr();
                consume(Token::Type::RP);                
                
                gs_expr->iter_expr = iter_expr;
                expr = gs_expr;

            } else if (tryConsume(Token::Type::APPLY)) {
                consume(Token::Type::LP);
                auto apply_expr = std::make_shared<fir::ApplyExpr>();
                apply_expr->target = expr;
                //apply_expr->input_function = parseIdent();
                apply_expr->input_function = parseFunctorExpr();
                consume(Token::Type::RP);
                expr = apply_expr;
            } else if (tryConsume(Token::Type::INIT)) {
                consume(Token::Type::LP);
                auto init_expr = std::make_shared<fir::InitExpr>();
                init_expr->target = expr;
                init_expr->input_function = parseFunctorExpr();
                consume(Token::Type::RP);
                expr = init_expr;
            } else if (tryConsume(Token::Type::PROC)) {
                consume(Token::Type::LP);
                auto process_expr = std::make_shared<fir::ProcExpr>();
                process_expr->target = expr;
                process_expr->input_function = parseFunctorExpr();
                consume(Token::Type::RP);
                expr = process_expr;
            } else if (peek().type == Token::Type::IDENT) {
                // transforming into builtin intrinsics (runtime libraries)
                auto ident = parseIdent();
                if (tryConsume(Token::Type::LP)) {
                    //make a  method call expression
                    auto method_call_expr = std::make_shared<fir::MethodCallExpr>();
                    method_call_expr->target = expr;
                    if (isIntrinsic(ident->ident)) {
                        ident->ident = "builtin_" + ident->ident;
                    }

                    method_call_expr->method_name = ident;
                    if (peek().type != Token::Type::RP) {
                        method_call_expr->args = parseExprParams();
                    }
                    consume(Token::Type::RP);
                    expr = method_call_expr;
                } else {
                    auto field_read = std::make_shared<fir::FieldReadExpr>();

                    field_read->setOrElem = expr;
                    field_read->field = ident;
                    expr = field_read;
                }
            } else {
                reportError(peek(), " error parsing FieldReadExpr");
                throw SyntaxError();
            }

        }
        return expr;
    }

// factor: ('(' expr ')') | call_expr | unnamed_tuple_read_expr
//       | named_tuple_read_expr | range_const | var_expr | tensor_literal
    fir::Expr::Ptr Parser::parseFactor() {
        switch (peek().type) {
            case Token::Type::LP: {
                auto parenExpr = std::make_shared<fir::ParenExpr>();

                const Token leftParenToken = consume(Token::Type::LP);
                parenExpr->setBeginLoc(leftParenToken);

                parenExpr->expr = parseExpr();

                const Token rightParenToken = consume(Token::Type::RP);
                parenExpr->setEndLoc(rightParenToken);

                return parenExpr;
            }
            case Token::Type::IDENT: {

                const std::string identStr = peek().str;

                if (decls.contains(identStr)) {
                    switch (decls.get(identStr)) {
                        case IdentType::FUNCTION:
                            // If the function is actually being called, then return a CallExpr, else treat the function name as a variable and return a VarExpr
                            // it could be stateless functor (aka normal function)
                            if (peek(1).type == Token::Type::LP){
                                return parseCallExpr();
                            }
                            break;
                        default:
                            break;
                    }
                }
                return parseVarExpr();
            }
            case Token::Type::INT_LITERAL:
            case Token::Type::FLOAT_LITERAL:
            case Token::Type::STRING_LITERAL:
            case Token::Type::TRUE:
            case Token::Type::FALSE:
            case Token::Type::LB:
            case Token::Type::LA:
                return parseTensorLiteral();
            default:
                reportError(peek(), "an expression");
                throw SyntaxError();
                break;
        }

        return fir::Expr::Ptr();
    }
// read_params: read_param {',' read_param}
    std::vector<fir::ReadParam::Ptr> Parser::parseReadParams() {
        std::vector<fir::ReadParam::Ptr> readParams;

        do {
            const fir::ReadParam::Ptr param = parseReadParam();
            readParams.push_back(param);
        } while (tryConsume(Token::Type::COMMA));

        return readParams;
    }

// read_param: ':' | expr
    fir::ReadParam::Ptr Parser::parseReadParam() {
        if (peek().type == Token::Type::COL) {
            auto slice = std::make_shared<fir::Slice>();

            const Token colToken = consume(Token::Type::COL);
            slice->setLoc(colToken);

            return slice;
        }

        auto param = std::make_shared<fir::ExprParam>();
        param->expr = parseExpr();

        return param;
    }
// expr_params: expr {',' expr}
    std::vector<fir::Expr::Ptr> Parser::parseExprParams() {
        std::vector<fir::Expr::Ptr> exprParams;

        do {
            const fir::Expr::Ptr param = parseExpr();
            exprParams.push_back(param);
        } while (tryConsume(Token::Type::COMMA));

        return exprParams;
    }
    fir::FuncExpr::Ptr Parser::parseFunctorExpr() {
        auto funcExpr = std::make_shared<fir::FuncExpr>();

        auto ident = parseIdent();

        std::vector<fir::Expr::Ptr> arguments;

        if (tryConsume(Token::Type::LB)){
            if (peek().type != Token::Type::RB) {
                do {
                    const fir::Expr::Ptr argument = parseExpr();
                    arguments.push_back(argument);
                } while (tryConsume(Token::Type::COMMA));
            }
            consume(Token::Type::RB);

        }

        funcExpr->args = arguments;
        funcExpr->name = ident;


        return funcExpr;


    }
// call_expr: ident '(' [expr_params] ')'
    fir::CallExpr::Ptr Parser::parseCallExpr() {
        auto call = std::make_shared<fir::CallExpr>();
        call->func = parseIdent();

        consume(Token::Type::LP);

        if (peek().type != Token::Type::RP) {
            call->args = parseExprParams();
        }

        const Token endToken = consume(Token::Type::RP);
        call->setEndLoc(endToken);

        return call;
    }
// var_expr: ident
    fir::VarExpr::Ptr Parser::parseVarExpr() {
        auto varExpr = std::make_shared<fir::VarExpr>();

        const Token identToken = consume(Token::Type::IDENT);
        varExpr->setLoc(identToken);
        varExpr->ident = identToken.str;

        return varExpr;
    }
// tensor_literal: INT_LITERAL | FLOAT_LITERAL | 'true' | 'false'
//               | STRING_LITERAL 
    fir::Expr::Ptr Parser::parseTensorLiteral() {
        fir::Expr::Ptr literal;
        switch (peek().type) {
            case Token::Type::INT_LITERAL: {
                auto intLiteral = std::make_shared<fir::IntLiteral>();

                const Token intToken = consume(Token::Type::INT_LITERAL);
                intLiteral->setLoc(intToken);
                intLiteral->val = intToken.num;

                literal = intLiteral;
                break;
            }
            case Token::Type::FLOAT_LITERAL: {
                auto floatLiteral = std::make_shared<fir::FloatLiteral>();

                const Token floatToken = consume(Token::Type::FLOAT_LITERAL);
                floatLiteral->setLoc(floatToken);
                floatLiteral->val = floatToken.fnum;

                literal = floatLiteral;
                break;
            }
            case Token::Type::TRUE: {
                auto trueLiteral = std::make_shared<fir::BoolLiteral>();

                const Token trueToken = consume(Token::Type::TRUE);
                trueLiteral->setLoc(trueToken);
                trueLiteral->val = true;

                literal = trueLiteral;
                break;
            }
            case Token::Type::FALSE: {
                auto falseLiteral = std::make_shared<fir::BoolLiteral>();

                const Token falseToken = consume(Token::Type::FALSE);
                falseLiteral->setLoc(falseToken);
                falseLiteral->val = false;

                literal = falseLiteral;
                break;
            }
            case Token::Type::STRING_LITERAL: {
                auto stringLiteral = std::make_shared<fir::StringLiteral>();

                const Token stringToken = consume(Token::Type::STRING_LITERAL);
                stringLiteral->setLoc(stringToken);
                stringLiteral->val = stringToken.str;

                literal = stringLiteral;
                break;
            }
            default:
                reportError(peek(), "a tensor literal");
                throw SyntaxError();
                break;
        }

        return literal;
    }
//********************************************************************//
//**************************** FuncDecl ******************************//
//********************************************************************//

// func_decl: 'func' ident arguments [results] stmt_block 'end'
    fir::FuncDecl::Ptr Parser::parseFuncDecl() {
        const unsigned originalLevel = decls.levels();

        try {
            auto funcDecl = std::make_shared<fir::FuncDecl>();

            const Token funcToken = peek();
            funcDecl->setBeginLoc(funcToken);

            consume(Token::Type::FUNC);

            funcDecl->name = parseIdent();

            decls.insert(funcDecl->name->ident, IdentType::FUNCTION);
            decls.scope();

            funcDecl->args = parseArguments();
            funcDecl->results = parseResults();

            decls.scope();
            funcDecl->body = parseStmtBlock();
            decls.unscope();

            decls.unscope();


            const Token endToken = consume(Token::Type::BLOCKEND);
            funcDecl->setEndLoc(endToken);

            return funcDecl;
        } catch (const SyntaxError &) {
            while (decls.levels() != originalLevel) {
                decls.unscope();
            }

            throw;
        }
    }
    // arguments: '(' [argument_decl {',' argument_decl}] ')'
    std::vector<fir::Argument::Ptr> Parser::parseArguments() {
        std::vector<fir::Argument::Ptr> arguments;

        consume(Token::Type::LP);
        if (peek().type != Token::Type::RP) {
            do {
                const fir::Argument::Ptr argument = parseArgumentDecl();
                arguments.push_back(argument);
            } while (tryConsume(Token::Type::COMMA));
        }
        consume(Token::Type::RP);

        return arguments;
    }

// argument_decl: ident_decl
    fir::Argument::Ptr Parser::parseArgumentDecl() {
        auto argDecl = std::make_shared<fir::Argument>();

        const auto identDecl = parseIdentDecl();
        argDecl->name = identDecl->name;
        argDecl->type = identDecl->type;

        return argDecl;
    }

// ident_decl: ident ':' type
    fir::IdentDecl::Ptr Parser::parseIdentDecl() {
        auto identDecl = std::make_shared<fir::IdentDecl>();

        identDecl->name = parseIdent();
        consume(Token::Type::COL);
        identDecl->type = parseType();

        const auto type = IdentType::OTHER;
        decls.insert(identDecl->name->ident, type);

        return identDecl;
    }

// results: ['->' (ident_decl | ('(' ident_decl {',' ident_decl} ')')]
    std::vector<fir::IdentDecl::Ptr> Parser::parseResults() {
        std::vector<fir::IdentDecl::Ptr> results;

        if (tryConsume(Token::Type::RARROW)) {
            if (tryConsume(Token::Type::LP)) {
                do {
                    const fir::IdentDecl::Ptr result = parseIdentDecl();
                    results.push_back(result);
                } while (tryConsume(Token::Type::COMMA));
                consume(Token::Type::RP);
            } else {
                const fir::IdentDecl::Ptr result = parseIdentDecl();
                results.push_back(result);
            }
        }

        return results;
    }


// stmt_block: {stmt}
    fir::StmtBlock::Ptr Parser::parseStmtBlock() {
        auto stmtBlock = std::make_shared<fir::StmtBlock>();

        while (true) {
            switch (peek().type) {
                case Token::Type::BLOCKEND:
                case Token::Type::ELIF:
                case Token::Type::ELSE:
                case Token::Type::END:
                    return stmtBlock;
                default: {
                    const fir::Stmt::Ptr stmt = parseStmt();
                    if (stmt) {
                        stmtBlock->stmts.push_back(stmt);
                    }
                    break;
                }
            }
        }
    }
// stmt: var_decl | const_decl | if_stmt | while_stmt | do_while_stmt
//     | for_stmt | print_stmt | apply_stmt | expr_or_assign_stmt | break_stmt
//    | # identifier # stmt
    fir::Stmt::Ptr Parser::parseStmt() {
        switch (peek().type) {
            case Token::Type::VAR:
                return parseVarDecl();
            case Token::Type::CONST:
                return parseConstDecl();
            case Token::Type::IF:
                return parseIfStmt();
            case Token::Type::WHILE:
                return parseWhileStmt();
            case Token::Type::DO:
                return parseDoWhileStmt();
            case Token::Type::FOR:
                return parseForStmt();
            case Token::Type::PRINT:
            case Token::Type::PRINTLN:
                return parsePrintStmt();
//            case Token::Type::APPLY:
//                return parseApplyStmt();
            case Token::Type::BREAK:
                return parseBreakStmt();
            case Token::Type::NUMBER_SIGN: {
                consume(Token::Type::NUMBER_SIGN);
                fir::Identifier::Ptr label = parseIdent();
                consume(Token::Type::NUMBER_SIGN);
                fir::Stmt::Ptr stmt = parseStmt();
                stmt->stmt_label = label->ident;
                return stmt;
            }
            default:
                return parseExprOrAssignStmt();
        }
    }
// DEPRECATED SIMIT grammar var_decl: 'var' ident (('=' expr) | (':' tensor_type ['=' expr])) ';'
// var_decl: 'var' ident (('=' expr) | (':' type ['=' expr])) ';'
// Similar to Const Declaration, we extend the grammar to support more than just tensor types

    fir::VarDecl::Ptr Parser::parseVarDecl() {
        try {
            auto varDecl = std::make_shared<fir::VarDecl>();

            const Token varToken = consume(Token::Type::VAR);
            varDecl->setBeginLoc(varToken);

            varDecl->name = parseIdent();
            if (tryConsume(Token::Type::COL)) {
                // Extend the grammar to support more than just tensor types
                // varDecl->type = parseTensorType();
                varDecl->type = parseType();
                if (tryConsume(Token::Type::ASSIGN)) {
                    if (peek().type == Token::Type::LC) {
                        varDecl->initVal = parseConstantVectorExpr();

                    } else {
                        varDecl->initVal = parseExpr();
                    }
                }
            } else {
                consume(Token::Type::ASSIGN);
                varDecl->initVal = parseExpr();
            }

            const Token endToken = consume(Token::Type::SEMICOL);
            varDecl->setEndLoc(endToken);

            decls.insert(varDecl->name->ident, IdentType::OTHER);

            return varDecl;
        } catch (const SyntaxError &) {
            skipTo({Token::Type::SEMICOL});
            consume(Token::Type::SEMICOL);

            return fir::VarDecl::Ptr();
        }
    }
// if_stmt: 'if' expr stmt_block else_clause 'end'
    fir::IfStmt::Ptr Parser::parseIfStmt() {
        try {
            auto ifStmt = std::make_shared<fir::IfStmt>();

            const Token ifToken = consume(Token::Type::IF);
            ifStmt->setBeginLoc(ifToken);

            ifStmt->cond = parseExpr();

            decls.scope();
            ifStmt->ifBody = parseStmtBlock();
            decls.unscope();

            decls.scope();
            ifStmt->elseBody = parseElseClause();
            decls.unscope();

            const Token endToken = consume(Token::Type::BLOCKEND);
            ifStmt->setEndLoc(endToken);

            return ifStmt;
        } catch (const SyntaxError &) {
            skipTo({Token::Type::ELIF, Token::Type::ELSE, Token::Type::BLOCKEND});

            parseElseClause();
            consume(Token::Type::BLOCKEND);

            return fir::IfStmt::Ptr();
        }
    }
// while_stmt: 'while' expr stmt_block 'end'
    fir::WhileStmt::Ptr Parser::parseWhileStmt() {
        try {
            auto whileStmt = std::make_shared<fir::WhileStmt>();

            const Token whileToken = consume(Token::Type::WHILE);
            whileStmt->setBeginLoc(whileToken);

            whileStmt->cond = parseExpr();

            decls.scope();
            whileStmt->body = parseStmtBlock();
            decls.unscope();

            const Token endToken = consume(Token::Type::BLOCKEND);
            whileStmt->setEndLoc(endToken);

            return whileStmt;
        } catch (const SyntaxError &) {
            skipTo({Token::Type::BLOCKEND});
            consume(Token::Type::BLOCKEND);

            return fir::WhileStmt::Ptr();
        }
    }

// do_while_stmt: 'do' stmt_block 'end' 'while' expr
    fir::DoWhileStmt::Ptr Parser::parseDoWhileStmt() {
        try {
            auto doWhileStmt = std::make_shared<fir::DoWhileStmt>();

            const Token doToken = consume(Token::Type::DO);
            doWhileStmt->setBeginLoc(doToken);

            decls.scope();
            doWhileStmt->body = parseStmtBlock();
            decls.unscope();

            consume(Token::Type::BLOCKEND);
            consume(Token::Type::WHILE);
            doWhileStmt->cond = parseExpr();

            return doWhileStmt;
        } catch (const SyntaxError &) {
            skipTo({Token::Type::BLOCKEND, Token::Type::ELIF, Token::Type::ELSE,
                    Token::Type::END, Token::Type::VAR, Token::Type::CONST,
                    Token::Type::IF, Token::Type::WHILE, Token::Type::DO,
                    Token::Type::FOR, Token::Type::PRINT, Token::Type::PRINTLN});

            return fir::DoWhileStmt::Ptr();
        }
    }
// for_stmt: 'for' ident 'in' for_domain stmt_block 'end'
    fir::ForStmt::Ptr Parser::parseForStmt() {
        try {
            auto forStmt = std::make_shared<fir::ForStmt>();

            const Token forToken = consume(Token::Type::FOR);
            forStmt->setBeginLoc(forToken);

            forStmt->loopVar = parseIdent();
            consume(Token::Type::IN);
            forStmt->domain = parseForDomain();

            decls.scope();
            decls.insert(forStmt->loopVar->ident, IdentType::OTHER);

            forStmt->body = parseStmtBlock();
            decls.unscope();

            const Token endToken = consume(Token::Type::BLOCKEND);
            forStmt->setEndLoc(endToken);

            return forStmt;
        } catch (const SyntaxError &) {
            skipTo({Token::Type::BLOCKEND});
            consume(Token::Type::BLOCKEND);

            return fir::ForStmt::Ptr();
        }
    }
// print_stmt: ('print' | 'println') expr {',' expr} ';'
    fir::PrintStmt::Ptr Parser::parsePrintStmt() {
        try {
            auto printStmt = std::make_shared<fir::PrintStmt>();

            if (peek().type == Token::Type::PRINT) {
                const Token printToken = consume(Token::Type::PRINT);
                printStmt->setBeginLoc(printToken);
                printStmt->printNewline = false;
            } else {
                const Token printlnToken = consume(Token::Type::PRINTLN);
                printStmt->setBeginLoc(printlnToken);
                printStmt->printNewline = true;
            }

            do {
                const fir::Expr::Ptr arg = parseExpr();
                printStmt->args.push_back(arg);
            } while (tryConsume(Token::Type::COMMA));

            const Token endToken = consume(Token::Type::SEMICOL);
            printStmt->setEndLoc(endToken);

            return printStmt;
        } catch (const SyntaxError &) {
            skipTo({Token::Type::SEMICOL});
            consume(Token::Type::SEMICOL);

            return fir::PrintStmt::Ptr();
        }
    }
    fir::BreakStmt::Ptr Parser::parseBreakStmt() {
        consume(Token::Type::BREAK);
        consume(Token::Type::SEMICOL);
        return std::make_shared<fir::BreakStmt>();
    }
// expr_or_assign_stmt: [[expr {',' expr} '='] expr] ';'
    fir::ExprStmt::Ptr Parser::parseExprOrAssignStmt() {
        try {
            fir::ExprStmt::Ptr stmt;

            auto peek_type = peek().type;
            if (peek().type != Token::Type::SEMICOL) {
                const fir::Expr::Ptr expr = parseExpr();

                switch (peek().type) {
                    case Token::Type::COMMA:
                    case Token::Type::ASSIGN: {
                        auto assignStmt = std::make_shared<fir::AssignStmt>();

                        assignStmt->lhs.push_back(expr);
                        while (tryConsume(Token::Type::COMMA)) {
                            const fir::Expr::Ptr expr = parseExpr();
                            assignStmt->lhs.push_back(expr);
                        }

                        consume(Token::Type::ASSIGN);
                        assignStmt->expr = parseExpr();

                        for (const auto lhs : assignStmt->lhs) {
                            if (fir::isa<fir::VarExpr>(lhs)) {
                                const std::string varName = fir::to<fir::VarExpr>(lhs)->ident;

                                if (!decls.contains(varName)) {
                                    decls.insert(varName, IdentType::OTHER);
                                }
                            }
                        }

                        stmt = assignStmt;
                        break;
                    }
                    case Token::Type::PLUS_REDUCE: {
                        stmt = parseReduceStmt(Token::Type::PLUS_REDUCE, fir::ReduceStmt::ReductionOp::SUM, expr);
                        break;
                    }
                    case Token::Type::SUB_REDUCE: {
                        stmt = parseReduceStmt(Token::Type::SUB_REDUCE, fir::ReduceStmt::ReductionOp::SUB, expr);
                        break;
                    }
                    case Token::Type::MIN_REDUCE: {
                        stmt = parseReduceStmt(Token::Type::MIN_REDUCE, fir::ReduceStmt::ReductionOp::MIN, expr);
                        break;
                    }
                    case Token::Type::MAX_REDUCE: {
                        stmt = parseReduceStmt(Token::Type::MAX_REDUCE, fir::ReduceStmt::ReductionOp::MAX, expr);
                        break;
                    }
                    default:
                        stmt = std::make_shared<fir::ExprStmt>();
                        stmt->expr = expr;
                        break;
                }
            }

            const Token endToken = consume(Token::Type::SEMICOL);
            if (stmt) {
                stmt->setEndLoc(endToken);
            }

            return stmt;
        } catch (const SyntaxError &) {
            skipTo({Token::Type::SEMICOL});
            consume(Token::Type::SEMICOL);

            return fir::ExprStmt::Ptr();
        }
    }
// else_clause: [('else' stmt_block) | ('elif' expr stmt_block else_clause)]
    fir::Stmt::Ptr Parser::parseElseClause() {
        switch (peek().type) {
            case Token::Type::ELSE:
                consume(Token::Type::ELSE);
                return parseStmtBlock();
            case Token::Type::ELIF: {
                try {
                    auto elifClause = std::make_shared<fir::IfStmt>();

                    const Token elifToken = consume(Token::Type::ELIF);
                    elifClause->setBeginLoc(elifToken);

                    elifClause->cond = parseExpr();

                    decls.scope();
                    elifClause->ifBody = parseStmtBlock();
                    decls.unscope();

                    decls.scope();
                    elifClause->elseBody = parseElseClause();
                    decls.unscope();

                    return elifClause;
                } catch (const SyntaxError &) {
                    skipTo({Token::Type::ELIF, Token::Type::ELSE, Token::Type::BLOCKEND});

                    decls.scope();
                    parseElseClause();
                    decls.unscope();

                    return fir::Stmt::Ptr();
                }
            }
            default:
                return fir::Stmt::Ptr();
        }
    }
    // for_domain: (expr ':' expr)
    fir::ForDomain::Ptr Parser::parseForDomain() {

        auto rangeDomain = std::make_shared<fir::RangeDomain>();

        rangeDomain->lower = parseExpr();
        if (tryConsume(Token::Type::COL)) {
            rangeDomain->upper = parseExpr();
            rangeDomain->getNghMode = false;
        }
        return rangeDomain;
    }
    fir::ReduceStmt::Ptr Parser::parseReduceStmt(Token::Type token_type,
                                                 fir::ReduceStmt::ReductionOp reduce_op,
                                                 fir::Expr::Ptr expr) {
        auto reduce_stmt = std::make_shared<fir::ReduceStmt>();
        reduce_stmt->reduction_op = reduce_op;
        reduce_stmt->lhs.push_back(expr);
        while (tryConsume(Token::Type::COMMA)) {
            const fir::Expr::Ptr expr = parseExpr();
            reduce_stmt->lhs.push_back(expr);
        }

        consume(token_type);
        reduce_stmt->expr = parseExpr();

        for (const auto lhs : reduce_stmt->lhs) {
            if (fir::isa<fir::VarExpr>(lhs)) {
                const std::string varName = fir::to<fir::VarExpr>(lhs)->ident;

                if (!decls.contains(varName)) {
                    decls.insert(varName, IdentType::OTHER);
                }
            }
        }
        return reduce_stmt;
    }
//********************************************************************//
//**************************** Utils ******************************//
//********************************************************************//

    void Parser::reportError(const Token &token, std::string expected) {
        std::stringstream errMsg;
        errMsg << "expected " << expected << " but got " << token.toString();

        const auto err = ParseError(token.lineBegin, token.colBegin,
                                    token.lineEnd, token.colEnd, errMsg.str());
        errors->push_back(err);
    }
    void Parser::skipTo(std::vector<Token::Type> types) {
        while (peek().type != Token::Type::END) {
            for (const auto type : types) {
                if (peek().type == type) {
                    return;
                }
            }
            tokens.skip();
        }
    }

    Token Parser::consume(Token::Type type) {
        const Token token = peek();

        if (!tokens.consume(type)) {
            reportError(token, Token::tokenTypeString(type));
            throw SyntaxError();
        }

        return token;
    }
    void Parser::initIntrinsics() {
        //intrinsics - builtin_func_name
        //decls - func_name
        // set up method call intrinsics

        // //TODO: this one might need to be removed
        // intrinsics_.push_back("sum");
        // intrinsics_.push_back("max");

        // //library functions for edgeset
        intrinsics_.push_back("getVertices");
        intrinsics_.push_back("getEdges");
        intrinsics_.push_back("migrate");
        intrinsics_.push_back("getOutDegrees");
        // intrinsics_.push_back("getOutDegreesUint");
        // intrinsics_.push_back("getOutDegree");
        intrinsics_.push_back("getNeighbors");
        intrinsics_.push_back("getOutEdges");
        // intrinsics_.push_back("relabel");
        intrinsics_.push_back("dst");
        intrinsics_.push_back("weight");
        // // library functions for vertexset
        intrinsics_.push_back("getProperty");
        intrinsics_.push_back("pull");
        intrinsics_.push_back("push");
        // intrinsics_.push_back("getVertexSetSize");
        // intrinsics_.push_back("addVertex");

        // //library functions for list
        // intrinsics_.push_back("append");
        // intrinsics_.push_back("pop");
        // intrinsics_.push_back("transpose");

        // set up function call intrinsics
        //decls.insert("fabs", IdentType::FUNCTION);
        decls.insert("startTimer", IdentType::FUNCTION);
        decls.insert("stopTimer", IdentType::FUNCTION);
        decls.insert("fabs", IdentType::FUNCTION);
        decls.insert("pow", IdentType::FUNCTION);
        //decls.insert("atoi", IdentType::FUNCTION);
       // decls.insert("floor", IdentType::FUNCTION);
        // decls.insert("log", IdentType::FUNCTION);
        // decls.insert("to_double", IdentType::FUNCTION);
        // decls.insert("max", IdentType::FUNCTION);
        // decls.insert("writeMin", IdentType::FUNCTION);
        // decls.insert("atomicAdd", IdentType::FUNCTION);
	    // decls.insert("getRandomOutNgh", IdentType::FUNCTION);
        // decls.insert("getRandomInNgh", IdentType::FUNCTION);
        // decls.insert("serialMinimumSpanningTree", IdentType::FUNCTION);
    }

}
