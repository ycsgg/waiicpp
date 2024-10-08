#pragma once

#include "object.hpp"
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

namespace environment {
using object::obj_ptr;
using std::pair;
using std::shared_ptr;
using std::string;
using std::unordered_map;
class Enviroment {
    private:
    unordered_map<string, obj_ptr> store;
    shared_ptr<Enviroment> outer;

    public:
    pair<bool, obj_ptr> get(const string &name) {
        auto iter = store.find(name);
        if (iter != store.end()) {
            return {true, iter->second};
        } else {
            if (outer != nullptr) {
                return outer->get(name);
            }
        }
        return {false, nullptr};
    }
    obj_ptr set(const string &name, obj_ptr value) {
        store[name] = value;
        return value;
    }
    Enviroment() {
    }
    Enviroment(shared_ptr<Enviroment> outer) : outer(outer) {
    }
};

typedef shared_ptr<Enviroment> env_ptr;

} // namespace environment
