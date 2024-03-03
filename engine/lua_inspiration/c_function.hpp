#pragma once
#ifndef __LUA_C_FUNCTION_
#define __LUA_C_FUNCTION_

#include <string>
#include "state.hpp"

class LuaCFunc {
private:
    lua_CFunction mCFunc;
    std::string mName;

public:
    void SetName(std::string name) { mName = name; }
    std::string GetName() { return mName; }

    void SetCFunc(lua_CFunction cfn) { mCFunc = cfn; }
    lua_CFunction GetCFunc() { return mCFunc; }
};

#endif