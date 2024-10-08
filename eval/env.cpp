#pragma once

#include "object.cpp"
#include <map>
#include <string>
#include <unordered_map>
#include <memory>

namespace environment {
using object::obj_ptr;
using std::pair;
using std::string;
using std::unordered_map;
using std::shared_ptr;
class Enviroment {
    private:
    unordered_map<string, obj_ptr> store;

    public:
    pair<bool, obj_ptr> get(const string &name) {
        auto iter = store.find(name);
        if (iter != store.end()) {
            return {true, iter->second};
        }
        return {false, nullptr};
    }
    obj_ptr set(const string &name, obj_ptr value) {
        store[name] = value;
        return value;
    }
};

typedef shared_ptr<Enviroment> env_ptr;

} // namespace enviroment
