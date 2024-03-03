#pragma once
#ifndef __LUA_COMPILER_
#define __LUA_COMPILER_

#include <vector>
#include <string>
#include "state.hpp"
#include "../core.hpp"

typedef void (*PFN_LuaCompileErrorCheck)(LuaState& l, int error); 

class LuaCode {
private:
    std::string mName;
    std::vector<uint8_t> mCode;
    static void luaCodeErrorCheck(LuaState &l, int error) {
        if(error != LUA_OK) {
            switch(error) {
                case LUA_ERRMEM:
                    ENG_ERR("LUA: Out of memory");
                    break;

                case LUA_ERRSYNTAX:
                    ENG_ERR("Stx LUA: " << lua_tostring(l.Get(), 1));
                    break;

                default:
                    ENG_ERR("Un LUA: Code" << std::to_string(error) << " : " << lua_tostring(l.Get(), 1));
                    break;
            }
        }
    }

public:
    void WriteCode(uint8_t* buff, size_t size) {
        uint8_t* end = (uint8_t*)buff + size;
        mCode.insert(mCode.end(), buff, end);
    }

    void UploadCode(LuaState &l) {
        lua_load(l.Get(), LuaCode::CodeReader, this, (const char*)mName.c_str(), nullptr);
    }

    static int CodeWriter(lua_State* l, const void* p, size_t size, void* u) {
        ((LuaCode*)u)->WriteCode((uint8_t*)p, size);
        return 0;
    }

    static const char* CodeReader(lua_State* l, void* data, size_t* size) {
        *size = ((LuaCode*)data)->GetSize();
        return ((LuaCode*)data)->GetCode();
    }

    const char* GetCode() { return (const char*)&mCode[0]; }
    int GetSize() { return mCode.size(); }
    std::string GetName() { return mName; }
    void SetName(std::string const& name) { mName = name; }

    static std::unique_ptr<LuaCode> CompileString(std::string name, std::string code, PFN_LuaCompileErrorCheck errorCheck = LuaCode::luaCodeErrorCheck) {
        std::unique_ptr<LuaCode> cbp = std::make_unique<LuaCode>();

        LuaState l;
        int res = luaL_loadstring(l.Get(), code.c_str());
        errorCheck(l, res);

        res = lua_dump(l.Get(), LuaCode::CodeWriter, (void*)cbp.get(), 0);
        errorCheck(l, res);

        cbp->SetName(name);
        return cbp;
    }

    static std::unique_ptr<LuaCode> CompileFile(std::string name, std::string filename, PFN_LuaCompileErrorCheck errorCheck = LuaCode::luaCodeErrorCheck) {
        std::unique_ptr<LuaCode> cbp = std::make_unique<LuaCode>();

        LuaState l;
        int res = luaL_loadfile(l.Get(), filename.c_str());
        errorCheck(l, res);

        res = lua_dump(l.Get(), LuaCode::CodeWriter, (void*)cbp.get(), 0);
        errorCheck(l, res);

        cbp->SetName(name);
        return cbp;
    }
};

#endif