#pragma once
#ifndef __LUA_TYPE_STRING_
#define __LUA_TYPE_STRING_

#include "type_base.hpp"

class LuaTypeString : public LuaTypeBase {
private:
    std::string mValue;

public:
    LuaTypeString(std::string value) : LuaTypeBase() { mValue = value; }
    ~LuaTypeString() {}

    virtual int GetTypeID() override { return LUA_TSTRING; }

    virtual std::string GetTypeName(LuaState& l) const override { return std::string(lua_typename(l.Get(), LUA_TSTRING)); }

    virtual void PushValue(LuaState& l) override { lua_pushstring(l.Get(), mValue.c_str()); }

    using LuaTypeBase::PopValue;
    virtual void PopValue(LuaState& l, int idx) override {
        if(lua_type(l.Get(), idx) == LUA_TSTRING) {
            mValue = lua_tostring(l.Get(), idx);
        }
    }

    virtual std::string ToString() const override {
        return mValue;
    }

    std::string GetValue() const { return mValue; }
    void SetValue(std::string value) { mValue = value; } 
};

#endif