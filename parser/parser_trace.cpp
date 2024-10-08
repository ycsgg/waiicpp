#pragma once

#include <iostream>
#include <string>

namespace parser {
class _defer_helper {
    private:
    std::string _msg;

    public:
    _defer_helper(std::string msg) : _msg(msg) {
        std::cout << "BEGIN: " << msg << std::endl;
    }
    ~_defer_helper() {
        std::cout << "END: " << _msg << std::endl;
    }
};

#define CONTACT(a,b) a##b
#define trace(msg) _defer_helper CONTACT(defer_,__LINE__)(msg)

} // namespace parser
