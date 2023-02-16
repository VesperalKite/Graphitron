//
// Created by Zheng Feng on 11/16/22.
//

#ifndef FACET_FRONTEND_H
#define FACET_FRONTEND_H

#include <vector>
#include <string>
#include <fstream>
#include <facet/frontend/fir_context.h>


namespace facet{
    class ParseError;

    class Frontend{
    public:
        int parseStream(std::istream &programStream, FIRContext* ctx, std::vector<ParseError> *errors);
    };
}


#endif //FACET_FRONTEND_H