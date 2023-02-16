//
// Created by Zheng Feng on 11/16/22.
//

#include <graphitron/frontend/frontend.h>
#include <graphitron/frontend/parser.h>


namespace graphitron{
        /// Parses, typechecks and turns a given Simit-formated stream into Simit IR.
    int Frontend::parseStream(std::istream &programStream, FIRContext *context, std::vector<ParseError> *errors) {

        // Lexical and syntactic analyses.
        TokenStream tokens = Scanner(errors).lex(programStream);
        fir::Program::Ptr program = Parser(errors).parse(tokens);

        // Only emit IR if no syntactic or semantic error was found.
        if (!errors->empty()) {
            std::cout << "Error in parsing: " << std::endl;
            for (auto & error : *errors){
                std::cout << error << std::endl;
            }
            std::stable_sort(errors->begin(), errors->end());
            return 1;
        }

        context->setProgram(program);

        return 0;
    }
}

