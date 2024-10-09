#pragma once

#include "object.cpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace object {
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::unordered_map;

obj_ptr len(const std::vector<obj_ptr> &args);

static unordered_map<string, BuiltinFunction> BUILTINS = {{"len", len}};

obj_ptr len(const std::vector<obj_ptr> &args) {
    if (args.size() != 1) {
        throw newError("function {} expected {} arguments, got {}", "len", 1,
                       args.size());
    }
    if (type(args[0]) == Str) {
        return make_shared<Integer>(
            dynamic_cast<String *>(args[0].get())->Value.length());
    }
    throw newError("argument to `len` not supported, got {}",
                   TypeToString(type(args[0])));
}

} // namespace object