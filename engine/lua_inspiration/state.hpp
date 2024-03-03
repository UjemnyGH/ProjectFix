#pragma once
#ifndef __LUA_STATE_
#define __LUA_STATE_

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <memory>
#include <iostream>

class LuaState {
private:
    std::unique_ptr<lua_State> mLuaState;

public:
    LuaState() {
        mLuaState = std::make_unique<lua_State>(luaL_newstate());
    }

    lua_State* Get() { return mLuaState.get(); }

    void DebugPrint(std::ostream &out) {
        int top = lua_gettop(Get());
        out << "Total on stack " << top << "\n";
        for (int i=1; i <= top; i++ ) {

            out << "[" << i << "] -> (" << lua_typename(Get(), lua_type(Get(), i)) << ") ";
                if (lua_type(Get(),i) == LUA_TSTRING) {
                out << lua_tostring(Get(), i);
            }
            out <<  "\n";
        }
    }

    ~LuaState() {
        lua_close(mLuaState.get());
    }
};

#endif