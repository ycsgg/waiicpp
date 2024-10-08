#pragma once

#include "../eval/eval.cpp"
#include "../lexer/lexer.cpp"
#include "../parser/parser.cpp"
#include <iostream>

namespace repl {
class Repl {
    public:
    Repl(std::istream &in, std::ostream &out) {
        std::string line;
        out << ">>";
        environment::env_ptr env = std::make_shared<environment::Enviroment>();
        while (getline(in, line)) {
            auto L = lexer::Lexer(line);
            auto P = parser::Parser(L);

            auto res = P.ParserProgram();
            if (P.errors.empty()) {
                try {
                    auto ptr = eval::Eval(res.get(), env);
                    if (ptr != object::_NULL) {
                        out << ptr->Inspect() << std::endl;
                    }
                } catch (object::ErrorObject &e) {
                    out << e.Inspect() << std::endl;
                }
            } else {
                // auto rpL = lexer::Lexer(line);
                // for (auto tok = rpL.NextToken(); tok.Type != token::END;
                //      tok = rpL.NextToken()) {
                //     tok.Output(out);
                // }
                for (auto v : P.errors) {
                    out << v << std::endl;
                }
            }
            out << ">>";
        }
    }
};
} // namespace repl