#pragma once
#ifndef __LUA_TYPE_USER_DATA_
#define __LUA_TYPE_USER_DATA_

#include <map>
#include "type_base.hpp"

class LuaTypeUserData : public LuaTypeBase {
private:
    void* mUserData;
    size_t mUserDataSize;
    std::map<std::string, lua_CFunction> mMetatable;
    virtual void _write_data() {}
    virtual void _read_data() {}

public:
    explicit LuaTypeUserData(size_t size) : LuaTypeBase(), mUserDataSize(size) {}

    virtual int GetTypeID() override { return LUA_TUSERDATA; }

    virtual std::string GetTypeName(LuaState& l) const override { return std::string(lua_typename(l.Get(), LUA_TUSERDATA)); }

    virtual void PushValue(LuaState& l) override {
        mUserData = lua_newuserdata(l.Get(), mUserDataSize);

        if(mMetatable.size() > 0) {
            lua_newtable(l.Get());

            for(const auto & p : mMetatable) {
                lua_pushcfunction(l.Get(), p.second);
                lua_setfield(l.Get(), -2, p.first.c_str());
            }

            lua_setmetatable(l.Get(), -2);
        }

        _write_data();
    }

    using LuaTypeBase::PopValue;
    virtual void PopValue(LuaState& l, int idx) override {
        if(lua_type(l.Get(), idx) == LUA_TUSERDATA) {
            void* userdata = lua_touserdata(l.Get(), idx);

            if(userdata == mUserData) {
                _read_data();
            }
        }
    }

    virtual std::string ToString() const override { return "userdata"; }

    void* GetRaw() const { return mUserData; }

    void AddMetaFn(std::string name, lua_CFunction fn) { mMetatable[name] = fn; }
};

#endif