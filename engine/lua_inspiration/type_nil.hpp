#pragma once
#ifndef __LUA_TYPE_NIL_
#define __LUA_TYPE_NIL_

#include "type_base.hpp"

class LuaTypeNil : public LuaTypeBase {
public:
    LuaTypeNil() : LuaTypeBase() {}
    ~LuaTypeNil() {}

    virtual int GetTypeID() override { return LUA_TNIL; }

    virtual std::string GetTypeName(LuaState& l) const override { return std::string(lua_typename(l.Get(), LUA_TNIL)); }

    virtual void PushValue(LuaState& l) override { lua_pushnil(l.Get()); }

    using LuaTypeBase::PopValue;
    virtual void PopValue(LuaState& l, int idx) override {
        if(lua_type(l.Get(), idx) == LUA_TNIL) {
            // Do nothing, thete should be error handling but no one implemented it so far
        }
    }

    virtual std::string ToString() const override {
        return "nil";
    }
};

#endif