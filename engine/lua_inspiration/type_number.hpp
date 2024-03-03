#pragma once
#ifndef __LUA_TYPE_NUMBER_
#define __LUA_TYPE_NUMBER_

#include "type_base.hpp"

class LuaTypeNumber : public LuaTypeBase {
private:
    double mValue;

public:
    LuaTypeNumber(double value) : LuaTypeBase() { mValue = value; }
    ~LuaTypeNumber() {}

    virtual int GetTypeID() override { return LUA_TNUMBER; }

    virtual std::string GetTypeName(LuaState& l) const override { return std::string(lua_typename(l.Get(), LUA_TNUMBER)); }

    virtual void PushValue(LuaState& l) override { lua_pushnumber(l.Get(), mValue); }

    using LuaTypeBase::PopValue;
    virtual void PopValue(LuaState& l, int idx) override {
        if(lua_type(l.Get(), idx) == LUA_TNUMBER) {
            mValue = lua_tonumber(l.Get(), idx);
        }
    }

    virtual std::string ToString() const override {
        return std::to_string(mValue);
    }

    double GetValue() const { return mValue; }
    void SetValue(double value) { mValue = value; } 
};

#endif