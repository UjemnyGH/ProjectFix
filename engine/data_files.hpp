#pragma once
#ifndef __DATA_FILES_
#define __DATA_FILES_

#include "core.hpp"
#include <string>
#include <fstream>
#include <unordered_map>

struct DataValue {
    std::string mVName;
    std::string mValue;
};

struct DataStructure {
    std::string mSName;
    std::unordered_map<std::string, DataValue> mSValues;
};

class DataFile {
public:
    std::unordered_map<std::string, DataStructure> mDataFile;

    void AddStruct(std::string structName) {
        mDataFile.insert(std::make_pair(structName, DataStructure()));
        mDataFile[structName].mSName = structName;
    }

    void AddValue(std::string structName, std::string valueName, std::string value) {
        mDataFile[structName].mSValues.insert(std::make_pair(valueName, DataValue()));
        mDataFile[structName].mSValues[valueName].mVName = valueName;
        mDataFile[structName].mSValues[valueName].mValue = value;
    }

    void SaveToFile(std::string filename) {
        std::string data_to_save;

        for(auto stc : mDataFile) {
            data_to_save += "@" + stc.second.mSName;

            for(auto vals : stc.second.mSValues) {
                data_to_save += "#" + vals.second.mVName + "=" + vals.second.mValue;
            }
        }

        std::ofstream f(filename, std::ios::binary);

        f.write(data_to_save.data(), data_to_save.size());

        f.close();
    }

    void LoadFromFile(std::string filename) {
        mDataFile.clear();

        std::ifstream f(filename, std::ios::binary);

        std::string struct_name;
        std::string var_name;
        std::string value;

        bool found_struct = false;
        bool found_name = false;
        bool found_value = false;

        while(!f.eof()) {
            uint8_t c = f.get();

            if(found_struct && c != '#') struct_name += c;
            else if(found_struct && c == '#') AddStruct(struct_name);

            if(found_name && c != '=') var_name += c;
            
            if(found_value && (c != '#' && c != '@')) value += c;
            else if(found_value && (c == '#' || c == '@')) AddValue(struct_name, var_name, value);

            if(c == '@') { found_struct = true; found_name = false; found_value = false; struct_name.clear(); }
            if(c == '#') { found_struct = false; found_name = true; found_value = false; var_name.clear(); }
            if(c == '=') { found_struct = false; found_name = false; found_value = true; value.clear(); }
        }

        if(!value.empty() && !var_name.empty())
            AddValue(struct_name, var_name, value);

        f.close();
    }
};

#endif