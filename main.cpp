#include "./eval/eval.cpp"
#include "./lexer/lexer.cpp"
#include "./parser/parser.cpp"
#include "./parser/parser_func.cpp"
#include "./repl/repl.cpp"
#include <format>
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
    if (argc == 1) {
        repl::Repl(cin, cout);
    }
    if (argc == 2) {
        ifstream file(argv[1]);
        if (!file.is_open()) {
            cout << "Could not open file: " << argv[1] << endl;
            return 1;
        }
        string content((istreambuf_iterator<char>(file)),
                       (istreambuf_iterator<char>()));
        auto *L = new lexer::Lexer(content);

        parser::Parser P(L);
        environment::env_ptr env = std::make_shared<environment::Enviroment>();
        auto Node = P.ParserProgram();
        if (P.errors.empty()) {
            try {
                auto ptr = eval::Eval(Node.get(), env);
            } catch (object::ErrorObject &e) {
                cout << e.Inspect();
            }
        } else {
            for (auto v : P.errors) {
                cout << v << endl;
            }
        }
    }
}