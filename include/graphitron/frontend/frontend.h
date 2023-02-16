//
// Created by Zheng Feng on 11/16/22.
//

#ifndef GRAPHITRON_FRONTEND_H
#define GRAPHITRON_FRONTEND_H

#include <vector>
#include <string>
#include <fstream>
#include <graphitron/frontend/fir_context.h>


namespace graphitron{
    class ParseError;

    class Frontend{
    public:
        int parseStream(std::istream &programStream, FIRContext* ctx, std::vector<ParseError> *errors);
    };
}


#endif //GRAPHITRON_FRONTEND_H