#pragma once
#ifndef __LUA_LIBRARY_
#define __LUA_LIBRARY_

#include <map>
#include <vector>
#include "c_function.hpp"

class LuaLibrary {
private:
    std::string mLibraryName;
    std::string mLibraryMetaName;

    std::map<std::string, LuaCFunc> mFunctions;
    std::map<std::string, LuaCFunc> mMethods;
    std::map<std::string, LuaCFunc> mMetaMethods;

public:
    LuaLibrary(std::string const& name) {
        mLibraryName = name;
        mLibraryMetaName = name;
    }

    LuaLibrary(std::string const& name, std::string const& metaName) {
        mLibraryName = name;
        mLibraryMetaName = metaName;
    }

    std::string GetName() { return mLibraryName; }
    std::string GetMetaName() { return mLibraryMetaName; }

    void SetName(std::string const& name) { mLibraryName = name; }

    inline bool MethodExist(std::string const& name) { return !(mMethods.find(name) == mMethods.end()); }
    inline bool FunctionExist(std::string const& name) { return !(mFunctions.find(name) == mFunctions.end()); }

    void AddMetaMethod(std::string const& name, lua_CFunction cfn, bool replace = false) {
        if(replace) mMetaMethods.erase(name);

        if(!MethodExist(name)) {
            std::unique_ptr<LuaCFunc> func = std::make_unique<LuaCFunc>(cfn);
            func->SetName(name);
            mMetaMethods.insert(std::make_pair(name, std::move(*func)));
        }
    }

    void AddMethod(std::string const& name, lua_CFunction cfn, bool replace = false) {
        if(replace) mMethods.erase(name);

        if(!MethodExist(name)) {
            std::unique_ptr<LuaCFunc> func = std::make_unique<LuaCFunc>(cfn);
            func->SetName(name);
            mMethods.insert(std::make_pair(name, std::move(*func)));
        }
    }

    void AddFunction(std::string const& name, lua_CFunction cfn, bool replace = false) {
        if(replace) mFunctions.erase(name);

        if(!MethodExist(name)) {
            std::unique_ptr<LuaCFunc> func = std::make_unique<LuaCFunc>(cfn);
            func->SetName(name);
            mFunctions.insert(std::make_pair(name, std::move(*func)));
        }
    }

    lua_CFunction GetMethod(std::string const& name) { return mMethods[name].GetCFunc(); }
    lua_CFunction GetFunction(std::string const& name) { return mFunctions[name].GetCFunc(); }

    void Register(LuaState &l) {
        lua_pushnil(l.Get());
        lua_setglobal(l.Get(), mLibraryName.c_str());

        luaL_newmetatable(l.Get(), mLibraryMetaName.c_str());

        std::vector<luaL_Reg> array_meta_methods;
        array_meta_methods.resize(mMetaMethods.size() + 1);
        int count = 0;

        for(auto & mm : mMetaMethods) {
            LuaCFunc& fn = mm.second;
            array_meta_methods[count].name = fn.GetName().c_str();
            array_meta_methods[count].func = fn.GetCFunc();
            count++;
        }

        array_meta_methods[count].name = nullptr;
        array_meta_methods[count].func = nullptr;

        std::vector<luaL_Reg> array_meth;
        array_meth.resize(mMethods.size() + 1);
        count = 0;

        for(auto & m : mMethods) {
            LuaCFunc fn = m.second;
            array_meth[count].name = fn.GetName().c_str();
            array_meth[count].func = fn.GetCFunc();
            count++;
        }

        array_meth[count].name = nullptr;
        array_meth[count].func = nullptr;

        std::vector<luaL_Reg> array_func;
        array_func.resize(mFunctions.size() + 1);
        count = 0;

        for(auto & f : mFunctions) {
            LuaCFunc fn = f.second;
            array_func[count].name = fn.GetName().c_str();
            array_func[count].func = fn.GetCFunc();
            count++;
        }

        array_func[count].name = nullptr;
        array_func[count].func = nullptr;

        luaL_setfuncs(l.Get(), array_meta_methods.data(), 0);

        luaL_newlibtable(l.Get(), array_meth.data());

        luaL_setfuncs(l.Get(), array_meth.data(), 0);
        lua_setfield(l.Get(), -2, "__index");
        lua_pop(l.Get(), 1);

        luaL_newlib(l.Get(), array_func.data());
        lua_setglobal(l.Get(), mLibraryName.c_str());
    }
};

#endif