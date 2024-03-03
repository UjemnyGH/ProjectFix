#pragma once
#ifndef __LUA_TYPE_TABLE_
#define __LUA_TYPE_TABLE_

#include "type_base.hpp"
#include <map>
#include <sstream>

#include "type_nil.hpp"
#include "type_string.hpp"
#include "type_number.hpp"
#include "type_boolean.hpp"
#include "type_number.hpp"

class LuaTypeTableKey {
private:
    bool mIsNumber;
    std::string mStringValue;
    int mIntValue;

public:
    explicit LuaTypeTableKey(int value) : mIsNumber(true), mStringValue(), mIntValue(value) {}
    explicit LuaTypeTableKey(std::string value) : mIsNumber(false), mStringValue(value), mIntValue(0) {}
    explicit LuaTypeTableKey(const char* value) : mIsNumber(false), mStringValue(value), mIntValue(0) {}

    bool IsNumber() { return mIsNumber; }

    std::string GetStringValue() const { return mStringValue; }
    int GetIntValue() const { return mIntValue; }

    std::string ToString() const {
        if(mIsNumber) {
            return std::to_string(mIntValue);
        }
        else {
            return mStringValue;
        }
    }

    friend bool operator<(LuaTypeTableKey const& lhs, LuaTypeTableKey const& rhs) {
        if(lhs.mIsNumber && rhs.mIsNumber) {
            return lhs.mIntValue < rhs.mIntValue;
        }
        else if(!lhs.mIsNumber && !rhs.mIsNumber) {
            return lhs.mStringValue < rhs.mStringValue;
        }
        else if(lhs.mIsNumber) {
            return true;
        }

        return false;
    }

    friend bool operator==(LuaTypeTableKey const& lhs, LuaTypeTableKey const& rhs) {
        if(lhs.mIsNumber != rhs.mIsNumber) {
            return false;
        }
        else if(lhs.mIsNumber) {
            return lhs.mIntValue == rhs.mIntValue;
        }

        return lhs.mStringValue == rhs.mStringValue;
    }

    friend std::ostream& operator<<(std::ostream& out, LuaTypeTableKey const& key) {
        if(key.mIsNumber) {
            out << std::to_string(key.mIntValue);
        }
        else {
            out << key.mStringValue;
        }


        return out;
    }
};

class LuaTypeTable : public LuaTypeBase {
private:
    using type = std::map<LuaTypeTableKey, std::shared_ptr<LuaTypeBase>>;

    type mValues;
    bool mIsArray;

public:
    LuaTypeTable() : LuaTypeBase(), mIsArray(true), mValues() {}
    ~LuaTypeTable() {}

    virtual int GetTypeID() override { return LUA_TTABLE; }

    virtual std::string GetTypeName(LuaState& l) const override { return std::string(lua_typename(l.Get(), LUA_TTABLE)); }

    virtual void PushValue(LuaState& l) override { 
        lua_newtable(l.Get());

        for(const auto & p : mValues) {
            LuaTypeTableKey& key = *((LuaTypeTableKey*)&p.first);
            p.second->PushValue(l);

            if(key.IsNumber()) {
                lua_seti(l.Get(), -2, key.GetIntValue());
            }
            else {
                lua_setfield(l.Get(), -2, key.GetStringValue().c_str());
            }
        }
    }

    using LuaTypeBase::PopValue;
    virtual void PopValue(LuaState& l, int idx) override {
        if(idx < 0) {
            idx = lua_gettop(l.Get()) + idx + 1;
        }
        
        if(lua_istable(l.Get(), idx) == 1) {
            mValues.clear();
            lua_pushnil(l.Get());
            while(lua_next(l.Get(), idx) != 0) {
                std::shared_ptr<LuaTypeBase> field;

                switch(lua_type(l.Get(), -1)) {
                    case LUA_TSTRING:
                        field = std::make_shared<LuaTypeString>("");
                        field->PopValue(l, -1);
                        break;
                    case LUA_TBOOLEAN:
                        field = std::make_shared<LuaTypeBoolean>(false);
                        field->PopValue(l, -1);
                        break;
                    case LUA_TNUMBER:
                        field = std::make_shared<LuaTypeNumber>(0);
                        field->PopValue(l, -1);
                        break;
                    case LUA_TTABLE:
                        field = std::make_shared<LuaTypeTable>();
                        field->PopValue(l, -1);
                        break;
                    default:
                        field = std::make_shared<LuaTypeString>(lua_typename(l.Get(), lua_type(l.Get(), -1)));
                        break;
                }

                if (lua_type(l.Get(), -2) == LUA_TSTRING) {
				SetValue(LuaTypeTableKey(lua_tostring(l.Get(), -2)), field);
                } else {
                    SetValue(LuaTypeTableKey(lua_tointeger(l.Get(), -2)), field);
                }

                lua_pop(l.Get(), 1);
            }
        }
    }

    virtual std::string ToString() const override {
        std::stringstream ss;

        if(mIsArray) {
            ss << "[";
        }
        else {
            ss << "{";
        }

        bool add_comma = false;

        for(const auto p : mValues) {
            if(add_comma) {
                ss << ", ";
            }
            else {
                add_comma = true;
            }

            if(!mIsArray) {
                ss << "\"" << p.first << "\" : ";
            }

            if(p.second->GetTypeID() == LUA_TSTRING) {
                ss << "\"" << p.second->ToString() << "\"";
            }
            else {
                ss << p.second->ToString();
            }
        }

        if(mIsArray) {
            ss << "]";
        }
        else {
            ss << "}";
        }

        return ss.str();
    }

    type GetValues() const { return mValues; }
    LuaTypeBase& GetValues(LuaTypeTableKey key) const { return *mValues.at(key); }
    void SetValue(LuaTypeTableKey key, std::shared_ptr<LuaTypeBase> value) {
        if(!key.IsNumber()) {
            mIsArray = false;
        }

        mValues[key] = value;
    } 
};

#endif