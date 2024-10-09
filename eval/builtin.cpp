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
obj_ptr first(const std::vector<obj_ptr> &args);
obj_ptr last(const std::vector<obj_ptr> &args);
obj_ptr rest(const std::vector<obj_ptr> &args);
obj_ptr append(const std::vector<obj_ptr> &args);


static unordered_map<string, BuiltinFunction> BUILTINS = {{"len", len},
                                                          {"first", first},
                                                          {"last", last},
                                                          {"rest", rest},
                                                          {"append", append}};

obj_ptr len(const std::vector<obj_ptr> &args) {
    if (args.size() != 1) {
        throw newError("function {} expected {} arguments, got {}", "len", 1,
                       args.size());
    }
    if (type(args[0]) == Str_Obj) {
        return make_shared<Integer>(
            dynamic_cast<String *>(args[0].get())->Value.length());
    }
    if (type(args[0]) == Array_Obj) {
        return make_shared<Integer>(
            dynamic_cast<Array *>(args[0].get())->Elements.size());
    }
    throw newError("argument to `len` not supported, got {}",
                   TypeToString(type(args[0])));
}

obj_ptr first(const std::vector<obj_ptr> &args) {
    if (args.size() != 1) {
        throw newError("function {} expected {} arguments, got {}", "first", 1,
                       args.size());
    }
    if (type(args[0]) == Array_Obj) {
        auto arr = dynamic_cast<Array *>(args[0].get());
        if (arr->Elements.size() > 0) {
            return arr->Elements[0];
        } else {
            return _NULL;
        }
    }
    if (type(args[0]) == Str_Obj) {
        auto str = dynamic_cast<String *>(args[0].get());
        if (str->Value.length() > 0) {
            return make_shared<String>(str->Value.substr(0, 1));
        } else {
            return _NULL;
        }
    }
    throw newError("argument to `first` not supported, got {}",
                   TypeToString(type(args[0])));
}

obj_ptr last(const std::vector<obj_ptr> &args) {
    if (args.size() != 1) {
        throw newError("function {} expected {} arguments, got {}", "last", 1,
                       args.size());
    }
    if (type(args[0]) == Array_Obj) {
        auto arr = dynamic_cast<Array *>(args[0].get());
        if (arr->Elements.size() > 0) {
            return arr->Elements[arr->Elements.size() - 1];
        } else {
            return _NULL;
        }
    }
    if (type(args[0]) == Str_Obj) {
        auto str = dynamic_cast<String *>(args[0].get());
        if (str->Value.length() > 0) {
            return make_shared<String>(
                str->Value.substr(str->Value.length() - 1, 1));
        } else {
            return _NULL;
        }
    }
    throw newError("argument to `last` not supported, got {}",
                   TypeToString(type(args[0])));
}
obj_ptr rest(const std::vector<obj_ptr> &args) {
    if (args.size() != 1) {
        throw newError("function {} expected {} arguments, got {}", "rest", 1,
                       args.size());
    }
    if (type(args[0]) == Array_Obj) {
        auto arr = dynamic_cast<Array *>(args[0].get());
        if (arr->Elements.size() > 0) {
            auto new_arr = arr->Elements;
            new_arr.erase(new_arr.begin());
            return make_shared<Array>(new_arr);
        } else {
            return _NULL;
        }
    }
    if (type(args[0]) == Str_Obj) {
        auto str = dynamic_cast<String *>(args[0].get());
        if (str->Value.length() > 0) {
            return make_shared<String>(str->Value.substr(1));
        } else {
            return _NULL;
        }
    }
    throw newError("argument to `rest` not supported, got {}",
                   TypeToString(type(args[0])));
}

obj_ptr append(const std::vector<obj_ptr> &args) {
    if (args.size() != 2) {
        throw newError("function {} expected {} arguments, got {}", "append", 2,
                       args.size());
    }
    if (type(args[0]) == Array_Obj) {
        auto arr = dynamic_cast<Array *>(args[0].get());
        auto new_arr = arr->Elements;
        new_arr.push_back(args[1]);
        return make_shared<Array>(new_arr);
    }
    throw newError("argument to `append` not supported, got {}",
                   TypeToString(type(args[0])));
}
} // namespace object