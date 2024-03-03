#pragma once
#ifndef __LUA_TYPE_BASE_
#define __LUA_TYPE_BASE_

#include <string>
#include "state.hpp"

class LuaTypeBase {
private:
    bool mIsGlobal;
    std::string mGlobalName;

public:
    LuaTypeBase() {
        mIsGlobal = false;
    }

    virtual ~LuaTypeBase() {}
    virtual int GetTypeID() {}
    virtual std::string GetTypeName(LuaState& l) const {}
    virtual void PushValue(LuaState& l) {}
    virtual void PopValue(LuaState& l, int idx) {}
    virtual std::string ToString() const {}
    
    void PopValue(LuaState& l) {
        PopValue(l, -1);
    }

    void PushGlobal(LuaState& l, std::string globalName) {
        PushValue(l);
        mGlobalName = globalName;
        mIsGlobal = true;

        lua_setglobal(l.Get(), mGlobalName.c_str());
    }

    void PopGlobal(LuaState& l) {
        if(mIsGlobal) {
            lua_getglobal(l.Get(), mGlobalName.c_str());
            PopValue(l);
            lua_pop(l.Get(), 1);
        }
    } 

    void PopGlobal(LuaState& l, std::string globalName) {
        mIsGlobal = true;
        mGlobalName = globalName;
        PopGlobal(l);
    }

    bool IsGlobal() const { return mIsGlobal; }
    std::string GetGlobalName() const { return mGlobalName; }
};

#endif