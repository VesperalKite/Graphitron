//
// Created by Zheng Feng on 11/17/22.
//

#include <cctype>
#include <string>
#include <cstdlib>
#include <iostream>

#include <graphitron/frontend/token.h>
#include <graphitron/utils/util.h>

namespace graphitron {
    std::string Token::tokenTypeString(Token::Type type){
        switch (type) {
            case Token::Type::INTX:
                return "intx";
            case Token::Type::INT:
                return "int";
            case Token::Type::FLOAT:
                return "float";
            case Token::Type::DOUBLE:
                return "double";
            case Token::Type::BOOL:
                return "bool";
            case Token::Type::STRING:
                return "string";
            case Token::Type::VECTOR:
                return "vector";

            case Token::Type::ELEMENT:
                return "element";
            case Token::Type::VAR:
                return "var";
            case Token::Type::CONST:
                return "const";
            case Token::Type::FUNC:
                return "func";
            case Token::Type::ITER:  
                return "iter";
            case Token::Type::GS:
                return "gs";
            case Token::Type::APPLY:
                return "apply";
            case Token::Type::GATHER:
                return "gather";
            case Token::Type::SCATTER:  
                return "scatter";
            case Token::Type::FILTER:
                return "filter";
            case Token::Type::INIT: 
                return "init";
            case Token::Type::PROC:
                return "process";

            case Token::Type::IF:
                return "if";    
            case Token::Type::ELIF:
                return "'elif'";
            case Token::Type::ELSE:
                return "'else'";
            case Token::Type::WHILE:
                return "'while'";
            case Token::Type::DO:  
                return "'do'";
            case Token::Type::FOR:
                return "'for'";
            case Token::Type::IN:
                return "'in'";
            case Token::Type::BLOCKEND:
                return "'end'";
            case Token::Type::RETURN:
                return "'return'";
            case Token::Type::PRINT:
                return "'print'";
            case Token::Type::PRINTLN:
                return "'println'";

            case Token::Type::VERTEX_SET:
                return "vertexset";
            case Token::Type::EDGE_SET:
                return "edgeset";
            case Token::Type::LOAD:
                return "load";
            case Token::Type::BREAK:
                return "break";
            case Token::Type::NUMBER_SIGN:
                return "#-label";
            
            case Token::Type::BIT_AND:
                return "'&'";
            case Token::Type::BIT_OR:  
                return "'|";

            case Token::Type::AND:
                return "'and'";
            case Token::Type::OR:
                return "'or'";
            case Token::Type::NOT:
                return "'not'";
            case Token::Type::XOR:
                return "'xor'";
            case Token::Type::TRUE:
                return "'true'";
            case Token::Type::FALSE:
                return "'false'";

            case Token::Type::IDENT:
                return "an identifier";

            case Token::Type::RARROW:
                return "'->'";
            case Token::Type::LP:
                return "'('";
            case Token::Type::RP:
                return "')'";
            case Token::Type::LB:
                return "'['";
            case Token::Type::RB:
                return "']'";
            case Token::Type::LC:
                return "'{'";
            case Token::Type::RC:
                return "'}'";
            case Token::Type::LA:
                return "'<'";
            case Token::Type::RA:
                return "'>'";
            case Token::Type::LS:  
                return "'<<'";
            case Token::Type::RS:  
                return "'>>";
            case Token::Type::EQ:
                return "'=='";
            case Token::Type::NE:
                return "'!='";
            case Token::Type::LE:
                return "'<='";
            case Token::Type::RE:
                return "'>='";
                
            case Token::Type::COMMA:
                return "','";
            case Token::Type::PERIOD:
                return "'.'";
            case Token::Type::COL:
                return "':'";
            case Token::Type::SEMICOL:
                return "';'";
    
            case Token::Type::ASSIGN:
                return "'='";
            case Token::Type::PLUS:
                return "'+'";
            case Token::Type::SUB:
                return "-";
            case Token::Type::MUL:
                return "*";
            case Token::Type::DIV:
                return "/";
            case Token::Type::PLUS_REDUCE:
                return "+=";
            case Token::Type::SUB_REDUCE: 
                return "-=";
            case Token::Type::MAX_REDUCE:
                return "max=";
            case Token::Type::MIN_REDUCE: 
                return "min=";

            case Token::Type::END: 
                return "end of file";
            case Token::Type::UNKNOWN:
                return "unknown";
            case Token::Type::INT_LITERAL:
                return "an integer literal";
            case Token::Type::FLOAT_LITERAL:
                return "a float literal";
            case Token::Type::STRING_LITERAL:
                return "a string literal";

            default:
                //unreachable;
                return "";
        }
    }

    std::string Token::toString() const {
        switch (type)
        {
        case Token::Type::INT_LITERAL:
            return "'" + std::to_string(num) + "'";
        case Token::Type::FLOAT_LITERAL:  
            return "'" + std::to_string(fnum) + "'";
        case Token::Type::STRING_LITERAL:  
            return "'\"" + util::escape(str) + "\"'";
        case Token::Type::IDENT: 
            return "'" + str + "'";
        default:
            return tokenTypeString(type);
        }
    }

    std::ostream &operator <<(std::ostream &out, const Token &token) {
        out << "(" << Token::tokenTypeString(token.type);
        switch (token.type) {
            case Token::Type::INT_LITERAL:
                out << ", " << token.num;
                break;
            case Token::Type::FLOAT_LITERAL:
                out << ", " << token.fnum;
                break;
            case Token::Type::STRING_LITERAL:
                out << ", \"" << token.str << "\"";
                break;
            case Token::Type::IDENT:
                out << ", " << token.str;
                break;
            default:
                break;
        }
        out << ", " << token.lineBegin << ":" << token.colBegin << "-"
            << token.lineEnd << ":" << token.colEnd << ")";
        return out;
    }

    void TokenStream::addToken(Token::Type type, unsigned line,
                               unsigned col, unsigned len) {
        Token newToken;

        newToken.type = type;
        newToken.lineBegin = line;
        newToken.colBegin = col;
        newToken.lineEnd = line;
        newToken.colEnd = col + len - 1;

        tokens.push_back(newToken);
    }

    bool TokenStream::consume(Token::Type type) {
        if (tokens.front().type == type) {
            tokens.pop_front();
            return true;
        }

        return false;
    }

    Token TokenStream::peek(unsigned k) const {
        if (k == 0) {
            return tokens.front();
        }

        std::list<Token>::const_iterator it = tokens.cbegin();
        for (unsigned i = 0; i < k && it != tokens.cend(); ++i, ++it) {}

        if (it == tokens.cend()) {
            Token endToken = Token();
            endToken.type = Token::Type::END;
            return endToken;
        }

        return *it;
    }

    bool TokenStream::contains(Token::Type tokenType) const {
        auto it = tokens.cbegin();
        while(it != tokens.cend()){
            Token foundToken = *it;
            if (foundToken.type == tokenType) return true;
            it++;
        }
        return false;

    }

    int TokenStream::findFirstOccurence(Token::Type tokenType) const {
        auto it = tokens.cbegin();

        int count = 0;
        while(it != tokens.cend()){
            Token foundToken = *it;
            if (foundToken.type == tokenType) return count;
            it++;
            count++;
        }
        return -1;

    }

    std::ostream &operator <<(std::ostream &out, const TokenStream &tokens) {
        for (auto it = tokens.tokens.cbegin(); it != tokens.tokens.cend(); ++it) {
            out << *it << std::endl;
        }
        return out;
    }
}