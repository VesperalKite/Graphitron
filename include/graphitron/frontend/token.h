//
// Created by Zheng Feng on 11/16/22.
//

#ifndef GRAPHITRON_TOKEN_H
#define GRAPHITRON_TOKEN_H

#include <list>
#include <sstream>
#include <string>

namespace graphitron {
    struct Token {
        enum class Type {
            INTX,//intx
            INT,//int
            FLOAT,//float
            DOUBLE,//double
            BOOL,//bool
            STRING,//string
            VECTOR,//vector

            ELEMENT,//element
            VAR,//var
            CONST,//const
            FUNC,//func
            ITER,//iter
            GS,//gs
            APPLY,//apply
            GATHER,//gather
            SCATTER,//scatter
            FILTER,//filter
            INIT,//init

            PROC,//process

            IF,//if
            ELIF,//elif
            ELSE,//else
            WHILE,//while
            DO,//do
            FOR,//for
            IN,//in
            BLOCKEND,//end
            RETURN,//return
            PRINT,//print
            PRINTLN,//println

            VERTEX_SET,//vertexset
            EDGE_SET,//edgeset
            LOAD,//load
            BREAK,//break
            NUMBER_SIGN,//#

            AND,//and
            OR,//or
            NOT,//not
            XOR,//xor
            TRUE,//true
            FALSE,//false

            IDENT,//identifier

            RARROW,//->
            LP,//(
            RP,//)
            LB,//[
            RB,//]
            LC,//{
            RC,//}
            LA,//<
            RA,//>
            LS,//<<
            RS,//>>
            EQ,//==
            NE,//!=
            LE,//<=
            RE,//>=


            COMMA,//,
            PERIOD,//.
            COL,//:
            SEMICOL,//;

            ASSIGN,//=
            PLUS,//+
            SUB,//-
            MUL,//*
            DIV,///
            PLUS_REDUCE,//+=

            END,
            UNKNOWN,
            INT_LITERAL,
            FLOAT_LITERAL,
            STRING_LITERAL,
        };
        Type type;

        union {
            int num;
            double fnum;
        };
        std::string str;
        unsigned    lineBegin;
        unsigned    colBegin;
        unsigned    lineEnd;
        unsigned    colEnd;

        static std::string tokenTypeString(Token::Type);

        std::string toString() const;

        friend std::ostream &operator <<(std::ostream &, const Token &);
    };

    struct TokenStream {
        void addToken(Token newToken) { tokens.push_back(newToken); }
        void addToken(Token::Type, unsigned, unsigned, unsigned = 1);

        Token peek(unsigned) const;

        bool contains(Token::Type tokenType) const;

        //Finds the first occurence of the token type from the stream of tokens and return its' relative index.
        int findFirstOccurence(Token::Type tokenType) const;

        void skip() { tokens.pop_front(); }
        bool consume(Token::Type);

        friend std::ostream &operator <<(std::ostream &, const TokenStream &);

    private:
        std::list<Token> tokens;
    };
}

#endif //GRAPHITRON_TOKEN_H