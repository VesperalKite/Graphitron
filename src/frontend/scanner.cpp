//
// Created by Zheng Feng on 11/16/22.
//

#include <graphitron/frontend/scanner.h>

namespace graphitron {

    Token::Type Scanner::getTokenType(const std::string token){
        if (token == "intx") return Token::Type::INTX;
        if (token == "int") return Token::Type::INT;
        if (token == "float") return Token::Type::FLOAT;
        if (token == "double") return Token::Type::DOUBLE;
        if (token == "bool") return Token::Type::BOOL;
        if (token == "string") return Token::Type::STRING;
        if (token == "vector") return Token::Type::VECTOR;
        if (token == "element") return Token::Type::ELEMENT;
        if (token == "var") return Token::Type::VAR;
        if (token == "const") return Token::Type::CONST;
        if (token == "func") return Token::Type::FUNC;
        if (token == "iter") return Token::Type::ITER;
        if (token == "gs") return Token::Type::GS;
        if (token == "apply") return Token::Type::APPLY;
        if (token == "gather") return Token::Type::GATHER;
        if (token == "scatter") return Token::Type::SCATTER;
        if (token == "filter") return Token::Type::FILTER;
        if (token == "init") return Token::Type::INIT;
        if (token == "process") return Token::Type::PROC;
        if (token == "if") return Token::Type::IF;
        if (token == "elif") return Token::Type::ELIF;
        if (token == "else") return Token::Type::ELSE;
        if (token == "while") return Token::Type::WHILE;
        if (token == "do") return Token::Type::DO;
        if (token == "for") return Token::Type::FOR;
        if (token == "in") return Token::Type::IN;
        if (token == "end") return Token::Type::BLOCKEND;
        if (token == "return") return Token::Type::RETURN;
        if (token == "print") return Token::Type::PRINT;
        if (token == "println") return Token::Type::PRINTLN;
        if (token == "&")   return Token::Type::BIT_AND;
        if (token == "|")   return Token::Type::BIT_OR;
        if (token == "and") return Token::Type::AND;
        if (token == "or") return Token::Type::OR;
        if (token == "not") return Token::Type::NOT;
        if (token == "xor") return Token::Type::XOR;
        if (token == "true") return Token::Type::TRUE;
        if (token == "false") return Token::Type::FALSE;
        if (token == "vertexset") return Token::Type::VERTEX_SET;
        if (token == "edgeset") return Token::Type::EDGE_SET;
        if (token == "load") return Token::Type::LOAD;
        if (token == "break") return Token::Type::BREAK;
        if (token == "#") return Token::Type::NUMBER_SIGN;

        // If string does not correspond to a keyword, assume it is an identifier.
        return Token::Type::IDENT;
    }


    TokenStream Scanner::lex(std::istream &programStream) {
        TokenStream tokens;
        unsigned line = 1;
        unsigned col = 1;

        //outer loop: to get token from the whole file
        while (programStream.peek() != EOF) {
            //tokens made up of alphas
            //start with alpha or #
            if (programStream.peek() == '#' || programStream.peek() == '_' || programStream.peek() == '&' || programStream.peek() == '|' || std::isalpha(programStream.peek())) {
                std::string tokenString(1, programStream.get());
                //# is a sign label
                if (tokenString != "#") {
                    while(programStream.peek() == '_' || std::isalnum(programStream.peek())) {
                        tokenString += programStream.get();    
                    } 
                }

                Token newToken;
                newToken.type = getTokenType(tokenString);
                newToken.lineBegin = line;
                newToken.colBegin = col;
                newToken.lineEnd = line;
                newToken.colEnd = col + tokenString.length() - 1;
                if (newToken.type == Token::Type::IDENT) {
                    newToken.str = tokenString;
                }
                tokens.addToken(newToken);
                col += tokenString.length();
            //if not start with alpha or #, it may be a symbol
            } else {
                switch (programStream.peek()) {
                    case '(':
                        programStream.get();
                        tokens.addToken(Token::Type::LP, line, col++); 
                        break;
                    case ')':
                        programStream.get();
                        tokens.addToken(Token::Type::RP, line, col++);
                        break;
                    case '[':
                        programStream.get();
                        tokens.addToken(Token::Type::LB, line, col++);
                        break;
                    case ']':
                        programStream.get();
                        tokens.addToken(Token::Type::RB, line, col++);
                        break;
                    case '{':
                        programStream.get();
                        tokens.addToken(Token::Type::LC, line, col++);
                        break;
                    case '}':
                        programStream.get();
                        tokens.addToken(Token::Type::RC, line, col++);
                        break;
                    case '<':
                        programStream.get();
                        if (programStream.peek() == '=') {
                            programStream.get();
                            tokens.addToken(Token::Type::LE, line, col, 2);
                            col += 2;
                        } else if (programStream.peek() == '<') {
                            programStream.get();
                            tokens.addToken(Token::Type::LS, line, col, 2);
                            col += 2;
                        } else {
                            tokens.addToken(Token::Type::LA, line, col++);
                        }
                        break;
                    case '>':
                        programStream.get();
                        if (programStream.peek() == '=') {
                            programStream.get();
                            tokens.addToken(Token::Type::RE, line, col, 2);
                            col += 2;
                        } else if (programStream.peek() == '>') {
                            programStream.get();
                            tokens.addToken(Token::Type::RS, line, col, 2);
                            col += 2;
                        } else {
                            tokens.addToken(Token::Type::RA, line, col++);
                        }
                        break;
                    case ',':
                        programStream.get();
                        tokens.addToken(Token::Type::COMMA, line, col++);
                        break;
                    case ':':
                        programStream.get();
                        tokens.addToken(Token::Type::COL, line, col++);
                        break;
                    case '.':
                        programStream.get();
                        tokens.addToken(Token::Type::PERIOD, line, col++);
                        break;
                    case ';':
                        programStream.get();
                        tokens.addToken(Token::Type::SEMICOL, line, col++);
                        break;
                    case '=':
                        programStream.get();
                        if (programStream.peek() == '=') {
                            programStream.get();
                            tokens.addToken(Token::Type::EQ, line, col, 2);
                            col += 2;
                        } else {
                            tokens.addToken(Token::Type::ASSIGN, line, col++);
                        }
                        break;
                    case '!':
                        programStream.get();
                        if (programStream.peek() == '=') {
                            programStream.get();
                            tokens.addToken(Token::Type::NE, line, col, 2);
                            col += 2;
                        } else {
                            reportError("unexpected symbol '!'", line, col++);
                            while (programStream.peek() != EOF &&
                                   !std::isspace(programStream.peek())) {
                                programStream.get();
                                ++col;
                            }
                        }
                        break;
                    case '%': {
                        programStream.get();
                        std::string comment;
                        while (programStream.peek() != '\n' &&
                                programStream.peek() != EOF) {
                            comment += programStream.get();
                        }
                        col += (comment.length() + 1);
                        // TODO: emit COMMENT token
                        break;
                    }
                    case '*':
                        programStream.get();
                        tokens.addToken(Token::Type::MUL, line, col++);
                        break;
                    case '/':
                        programStream.get();
                        tokens.addToken(Token::Type::DIV, line, col++);
                        break;
                    case '+':
                        programStream.get();

                        if (programStream.peek() == '=') {
                            // += token is plusreduce
                            programStream.get();
                            tokens.addToken(Token::Type::PLUS_REDUCE, line, col, 2);
                        } else {
                            tokens.addToken(Token::Type::PLUS, line, col++);
                        }

                        break;
                    case '-':
                        programStream.get();
                        if (programStream.peek() == '>') {
                            programStream.get();
                            tokens.addToken(Token::Type::RARROW, line, col, 2);
                            col += 2;
                        } else {
                            tokens.addToken(Token::Type::SUB, line, col++);
                        }
                        break;
                    case '"': {
                        Token newToken;
                        newToken.type = Token::Type::STRING_LITERAL;
                        newToken.lineBegin = line;
                        newToken.colBegin = col;

                        programStream.get();
                        ++col;

                        while (programStream.peek() != EOF && programStream.peek() != '"') {
                            if (programStream.peek() == '\\') {
                                programStream.get();

                                std::string escapedChar = "";
                                switch (programStream.peek()) {
                                    case 'a':
                                        escapedChar = "\a";
                                        break;
                                    case 'b':
                                        escapedChar = "\b";
                                        break;
                                    case 'f':
                                        escapedChar = "\f";
                                        break;
                                    case 'n':
                                        escapedChar = "\n";
                                        break;
                                    case 'r':
                                        escapedChar = "\r";
                                        break;
                                    case 't':
                                        escapedChar = "\t";
                                        break;
                                    case 'v':
                                        escapedChar = "\v";
                                        break;
                                    case '\\':
                                        escapedChar = "\\";
                                        break;
                                    case '\'':
                                        escapedChar = "\'";
                                        break;
                                    case '"':
                                        escapedChar = "\"";
                                        break;
                                    case '?':
                                        escapedChar = "\?";
                                        break;
                                    default:
                                        reportError("unrecognized escape sequence", line, col);
                                        ++col;
                                        break;
                                }

                                if (escapedChar != "") {
                                    newToken.str += escapedChar;
                                    programStream.get();
                                    col += 2;
                                }
                            } else {
                                newToken.str += programStream.get();
                                ++col;
                            }
                        }

                        newToken.lineEnd = line;
                        newToken.colEnd = col;
                        tokens.addToken(newToken);

                        if (programStream.peek() == '"') {
                            programStream.get();
                            ++col;
                        } else {
                            reportError("unclosed string literal", line, col);
                        }
                        break;
                    }
                    case '\r':
                        programStream.get();
                        if (programStream.peek() == '\n') {
                            programStream.get();
                        }
                        ++line;
                        col = 1;
                        break;
                    case '\v':
                    case '\f':
                    case '\n':
                        programStream.get();
                        ++line;
                        col = 1;
                        break;
                    case ' ':
                    case '\t':
                        programStream.get();
                        ++col;
                        break;
                    default: {
                        Token newToken;
                        newToken.type = Token::Type::INT_LITERAL;
                        newToken.lineBegin = line;
                        newToken.colBegin = col;
                        //if start with . or not a num
                        if (programStream.peek() != '.' && !std::isdigit(programStream.peek())) {
                            std::stringstream errMsg;
                            errMsg << "unexpected symbol '"
                                   << (char) programStream.peek() << "'";
                            reportError(errMsg.str(), line, col);

                            while (programStream.peek() != EOF &&
                                   !std::isspace(programStream.peek())) {
                                programStream.get();
                                ++col;
                            }
                            break;
                        }

                        std::string tokenString;
                        while (std::isdigit(programStream.peek())) {
                            tokenString += programStream.get();
                            ++col;
                        }
                        //if a num with ., it is a float
                        if (programStream.peek() == '.') {
                            newToken.type = Token::Type::FLOAT_LITERAL;
                            tokenString += programStream.get();
                            ++col;

                            if (!std::isdigit(programStream.peek())) {
                                std::stringstream errMsg;
                                errMsg << "unexpected symbol '"
                                       << (char) programStream.peek() << "'";
                                reportError(errMsg.str(), line, col);

                                while (programStream.peek() != EOF &&
                                       !std::isspace(programStream.peek())) {
                                    programStream.get();
                                    ++col;
                                }
                                break;
                            }
                            tokenString += programStream.get();
                            ++col;

                            while (std::isdigit(programStream.peek())) {
                                tokenString += programStream.get();
                                ++col;
                            }
                        }

                        if (programStream.peek() == 'e' || programStream.peek() == 'E') {
                            newToken.type = Token::Type::FLOAT_LITERAL;
                            tokenString += programStream.get();
                            ++col;

                            if (programStream.peek() == '+' || programStream.peek() == '-') {
                                tokenString += programStream.get();
                                ++col;
                            }

                            if (!std::isdigit(programStream.peek())) {
                                std::stringstream errMsg;
                                errMsg << "unexpected symbol '"
                                       << (char) programStream.peek() << "'";
                                reportError(errMsg.str(), line, col);

                                while (programStream.peek() != EOF &&
                                       !std::isspace(programStream.peek())) {
                                    programStream.get();
                                    ++col;
                                }
                                break;
                            }
                            tokenString += programStream.get();
                            ++col;

                            while (std::isdigit(programStream.peek())) {
                                tokenString += programStream.get();
                                ++col;
                            }
                        }

                        char *end;
                        if (newToken.type == Token::Type::INT_LITERAL) {
                            newToken.num = std::strtol(tokenString.c_str(), &end, 0);
                        } else {
                            newToken.fnum = std::strtod(tokenString.c_str(), &end);
                        }
                        newToken.lineEnd = line;
                        newToken.colEnd = col - 1;
                        tokens.addToken(newToken);
                        break;
                    }
                }
            }
        }
        //read all file
        tokens.addToken(Token::Type::END, line, col);
        return tokens;
    }

    void Scanner::printDebugInfo(const std::string &token_string, TokenStream &token_stream) {
        util::printDebugInfo(("current token string: " + token_string));
        std::stringstream ss;
        ss << token_stream;
        util::printDebugInfo((ss.str() + "\n ----- \n"));
    }
}
