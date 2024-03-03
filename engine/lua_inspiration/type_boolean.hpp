#pragma once
#ifndef __LUA_TYPE_BOOLEAN_
#define __LUA_TYPE_BOOLEAN_

#include "type_base.hpp"

class LuaTypeBoolean : public LuaTypeBase {
private:
    bool mValue;

public:
    explicit LuaTypeBoolean(bool value) : LuaTypeBase() { mValue = value; }
    virtual ~LuaTypeBoolean() override {}

    virtual int GetTypeID() override { return LUA_TBOOLEAN; }

    virtual std::string GetTypeName(LuaState& l) const override { return std::string(lua_typename(l.Get(), LUA_TBOOLEAN)); }

    virtual void PushValue(LuaState& l) override { lua_pushboolean(l.Get(), mValue); }

    using LuaTypeBase::PopValue;
    virtual void PopValue(LuaState& l, int idx) override {
        if(lua_type(l.Get(), idx) == LUA_TBOOLEAN) {
            mValue = lua_toboolean(l.Get(), idx);
        }
    }

    virtual std::string ToString() const override {
        return mValue ? "true" : "false";
    }

    bool GetValue() const { return mValue; }
    void SetValue(bool value) { mValue = value; } 

};

#endif