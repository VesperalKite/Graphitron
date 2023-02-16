//
// Created by Zheng Feng on 11/16/22.
//

#ifndef FACET_SCANNER_H
#define FACET_SCANNER_H


#include <sstream>
#include <string>
#include <vector>
#include <facet/frontend/token.h>
#include <facet/frontend/error.h>

namespace facet {
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
#endif //FACET_SCANNER_H