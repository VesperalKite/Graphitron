//
// Created by Zheng Feng on 11/16/22.
//

#ifndef GRAPHITRON_SCANNER_H
#define GRAPHITRON_SCANNER_H


#include <sstream>
#include <string>
#include <vector>
#include <graphitron/frontend/token.h>
#include <graphitron/frontend/error.h>

namespace graphitron {
    class Scanner {
    public:  
        Scanner(std::vector<ParseError> *errors) : errors(errors){}

        TokenStream lex(std::istream &);

    private:  
        static Token::Type getTokenType(const std::string);

        void reportError(std::string msg, unsigned line, unsigned col){
            errors->push_back(ParseError(line, col, line, col, msg));
        }

    private:
        std::vector<ParseError> *errors;
        void printDebugInfo(const std::string & tokenString, TokenStream & tokenStream);
    };
}
#endif //GRAPHITRON_SCANNER_H