#pragma once
#ifndef __LUA_CODE_REGISTRY_
#define __LUA_CODE_REGISTRY_

#include <map>
#include "code_compiler.hpp"

class LuaCodeRegistry {
private:
    std::map<std::string, LuaCode> mRegistry;

public:
    inline bool Exist(std::string const& name) {
        return !(mRegistry.find(name) == mRegistry.end());
    }

    void CompileAndAddString(std::string const& name, std::string const& code, bool recompile = false) {
        if(!Exist(name) || recompile) {
            std::unique_ptr<LuaCode> lc = LuaCode::CompileString(name, code);

            mRegistry[name] = std::move(*lc);
        }
    }

    void CompileAndAddFile(std::string const& name, std::string const& filename, bool recompile = false) {
        if(!Exist(name) || recompile) {
            std::unique_ptr<LuaCode> lc = LuaCode::CompileFile(name, filename);

            mRegistry[name] = std::move(*lc);
        }
    }
};

#endif