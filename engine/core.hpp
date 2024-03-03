#pragma once
#ifndef __CORE_
#define __CORE_

#include <map>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <cmath>

#define ENG_ERR_C(msg) std::cerr << "\x1b[41m[ ERROR ]\x1b[31m\x1b[40m:\t" << __FILE__ " line " << __LINE__ << " in function \"" << __FUNCTION__ << "()\" > \x1b[37m" << msg << std::endl; exit(1);
#define ENG_ERR(msg) std::cerr << "\x1b[41m[ ERROR ]\x1b[31m\x1b[40m:\t" << __FILE__ " line " << __LINE__ << " in function \"" << __FUNCTION__ << "()\" > \x1b[37m" << msg << std::endl;
#define ENG_WARN(msg) std::cerr << "\x1b[43m[WARNING]\x1b[33m\x1b[40m:\t" << __FILE__ " line " << __LINE__ << " in function \"" << __FUNCTION__ << "()\" > \x1b[37m" << msg << std::endl;
#ifndef HIDE_INFO
#define ENG_INFO(msg) std::cerr << "\x1b[44m[ INFO ]\x1b[34m\x1b[40m:\t" << __FILE__ " line " << __LINE__ << " in function \"" << __FUNCTION__ << "()\" > \x1b[37m" << msg << std::endl;
#else
#define ENG_INFO(msg)
#endif

#ifdef DEBUG
#define ENG_DEBUG(msg) std::cerr << "\x1b[42m[ DEBUG ]\x1b[32m\x1b[40m:\t" << __FILE__ " line " << __LINE__ << " in function \"" << __FUNCTION__ << "()\" > \x1b[37m" << msg << std::endl;
#else
#define ENG_DEBUG(msg)
#endif

#define SHADER_ASSET(name) ("assets/shaders/"#name)
#define MODEL_ASSET(name) ("assets/models/"#name)
#define TEXTURE_ASSET(name) ("assets/textures/"#name)

int gTextureSamplers[32] = {
    0,  1,  2,  3,  4,  5,  6,  7, 
    8,  9,  10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 
    24, 25, 26, 27, 28, 29, 30, 31
};

enum EObjectCommonType {
    EObjectCommonType_Object,
    EObjectCommonType_Actor,
    EObjectCommonType_Camera,
    EObjectCommonType_Renderer,
    EObjectCommonType_Button,
    EObjectCommonType_TextRenderer,
    EObjectCommonType_Scene,
    EObjectCommonType_UIComponent,
};

class EObject {
protected:
    std::string mName = "Object";
    std::string mTag = "DEFAULT";
    std::string mLayer = "DEFEAULT";
    std::string mType = "TYPE_NULL";
    EObject* mParent = nullptr;
    int mCommonType = EObjectCommonType_Object;

public:
    std::vector<EObject*> mChilds;

    virtual void ShouldAwake() {}
    virtual void ShouldStart() {}
    virtual void ShouldUpdate() {}
    virtual void ShouldFixedUpdate() {}
    virtual void ShouldLateUpdate() {}

    EObject(std::string name, std::string type) : mName(name), mType(type) {}

    inline void SetTag(std::string tag) { mTag = tag; }
    inline void SetLayer(std::string layer) { mLayer = layer; }
    inline void SetName(std::string name) { mName = name; }
    inline void SetParent(EObject *parent) { mParent = parent; }
    inline void AddChild(EObject *child) { mChilds.push_back(child); }

    inline int GetCommonType() { return mCommonType; }
    inline std::string GetTag() { return mTag; }
    inline std::string GetLayer() { return mLayer; }
    inline std::string GetName() { return mName; }
    inline std::string GetType() { return mType; }

    inline std::string* GetTagPtr() { return &mTag; }
    inline std::string* GetLayerPtr() { return &mLayer; }
    inline std::string* GetNamePtr() { return &mName; }
    inline std::string* GetTypePtr() { return &mType; }

    inline EObject* GetPartent() { return mParent; }

    inline bool operator==(EObject object) {
        return mName == object.GetName() || mTag == object.GetTag() || mLayer == object.GetLayer();
    }

    inline bool operator==(std::string value) {
        return mName == value || mTag == value || mLayer == value;
    }
};

class EObjectHandler {
private:
    std::vector<EObject*> mObjectsPtr;

public:
    EObjectHandler() {}

    inline std::vector<EObject*>::iterator Begin() { return mObjectsPtr.begin(); }
    inline std::vector<EObject*>::iterator End() { return mObjectsPtr.end(); }
    inline std::vector<EObject*>::size_type Size() { return mObjectsPtr.size(); }

    inline EObject* operator[](size_t n) { return mObjectsPtr[n]; }

    inline void operator()(EObject* pObject) { AddObjectPtr(pObject); }
    inline std::vector<EObject*> operator()(std::string name) { return FindObjectByName(name); }

    inline void AddObjectPtr(EObject* pObject) {
        mObjectsPtr.push_back(pObject);
    }

    inline std::vector<EObject*> FindObjectByName(std::string name) {
        std::vector<EObject*> objects;

        for(EObject* obj : mObjectsPtr) {
            if(obj->GetName() == name) {
                objects.push_back(obj);
            }
        }

        return objects;
    }

    inline std::vector<EObject*> FindObjectByType(std::string type) {
        std::vector<EObject*> objects;

        for(EObject* obj : mObjectsPtr) {
            if(obj->GetType() == type) {
                objects.push_back(obj);
            }
        }

        return objects;
    }

    inline std::vector<EObject*> FindObjectByTag(std::string tag) {
        std::vector<EObject*> objects;

        for(EObject* obj : mObjectsPtr) {
            if(obj->GetTag() == tag) {
                objects.push_back(obj);
            }
        }

        return objects;
    }

    inline std::vector<EObject*> FindObjectByLayer(std::string layer) {
        std::vector<EObject*> objects;

        for(EObject* obj : mObjectsPtr) {
            if(obj->GetLayer() == layer) {
                objects.push_back(obj);
            }
        }

        return objects;
    }

    inline void RemoveObjectsByName(std::string name) {
        for(int i = 0; i < mObjectsPtr.size(); i++) {
            if(mObjectsPtr[i]->GetName() == name) {
                mObjectsPtr.erase(mObjectsPtr.begin() + i);
            }
        }
    }

    inline void RemoveObjectsByTag(std::string tag) {
        for(int i = 0; i < mObjectsPtr.size(); i++) {
            if(mObjectsPtr[i]->GetTag() == tag) {
                mObjectsPtr.erase(mObjectsPtr.begin() + i);
            }
        }
    }

    inline void RemoveObjectsByLayer(std::string layer) {
        for(int i = 0; i < mObjectsPtr.size(); i++) {
            if(mObjectsPtr[i]->GetLayer() == layer) {
                mObjectsPtr.erase(mObjectsPtr.begin() + i);
            }
        }
    }
    
} gEObjectHandler;

/**
 * @brief AutoMap template class
 * 
 * @tparam T 
 */
template<class T>
class AutoMap {
protected:
    /**
     * @brief Map with named variables
     * 
     */
    std::map<std::string, T> mElements;

public:

    /**
     * @brief Get entire map and return pointer to it
     * 
     * @return std::map<std::string, T>*
     */
    std::map<std::string, T>* operator()() { return &mElements; }

    /**
     * @brief Gets exact element in map and return pointer to it
     * 
     * @param name 
     * @return T*
     */
    T* operator()(std::string name) { return &mElements.at(name); }
};

std::map<char, int> gBase64Charmap = {
    {'A', 00},  {'B', 01},  {'C', 02},  {'D', 03},  {'E', 04},  {'F', 05},  {'G', 06},  {'H', 07},
    {'I', 010}, {'J', 011}, {'K', 012}, {'L', 013}, {'M', 014}, {'N', 015}, {'O', 016}, {'P', 017},
    {'Q', 020}, {'R', 021}, {'S', 022}, {'T', 023}, {'U', 024}, {'V', 025}, {'W', 026}, {'X', 027},
    {'Y', 030}, {'Z', 031}, {'a', 032}, {'b', 033}, {'c', 034}, {'d', 035}, {'e', 036}, {'f', 037},
    {'g', 040}, {'h', 041}, {'i', 042}, {'j', 043}, {'k', 044}, {'l', 045}, {'m', 046}, {'n', 047},
    {'o', 050}, {'p', 051}, {'q', 052}, {'r', 053}, {'s', 054}, {'t', 055}, {'u', 056}, {'v', 057},
    {'w', 060}, {'x', 061}, {'y', 062}, {'z', 063}, {'0', 064}, {'1', 065}, {'2', 066}, {'3', 067},
    {'4', 070}, {'5', 071}, {'6', 072}, {'7', 073}, {'8', 074}, {'9', 075}, {'+', 076}, {'/', 077}
};

int CharsToInt(const char* src, int len) {
    int result = 0;
    bool negative = false;
 
    for(size_t i = 0; i < strlen(src) && i < (size_t)len; i++) {
        if(src[i] >= '0' && src[i] <= '9') {
            result *= 10;
            result += src[i] - '0';
        }
        else if(src[i] == '-') {
            negative = true;
        }
    }

    return result * (negative ? (-1) : 1);
}

float CharsToFloat(const char* src) {
    int dot = std::string(src).find('.');

    float result = CharsToInt(src, dot) + (CharsToInt(src + dot, strlen(src)) / pow(10, strlen(src) - dot));

    return result;
}

int CharToHex(const char c) {
    return c >= '0' && c <= '9' ? c - '0' : (c >= 'A' && c <= 'F' ? c - 'A' : (c >= 'a' && c <= 'f' ? c - 'a' : -1));
}

template<class T>
std::vector<T> JoinToVector(std::vector<std::vector<T>> vecs) {
    std::vector<T> joined_vecs;

    for(std::vector<T> vec : vecs) {
        std::copy(vec.begin(), vec.end(), std::back_inserter(joined_vecs));
    }

    return joined_vecs;
}

std::string DecodeBase64AsChars(std::string chars) {
    std::string decoded;

    uint32_t shift = 2;

    for(size_t i = 1; i < chars.length(); i++) {
        int32_t first = gBase64Charmap[chars.c_str()[i - 1]];
        int32_t second = gBase64Charmap[chars.c_str()[i]];

        //printf("Iter: %d,\tFirst: %d, Second: %d, F shift L by %d: %d, S shift R by %d: %d, F s + S s: %d\n", i, first, second, shift, ((first << shift) << 24) >> 24, 6 - shift, ((second >> 6 - shift) << 24) >> 24, ((first << shift) + (second >> (6 - shift)) << 24) >> 24);

        unsigned char ch = (((first << shift) + (second >> (6 - shift))) << 24) >> 24;

        if(/*((first << shift) << 24) >> 24 >= 0 && ((second >> 6 - shift) << 24) >> 24 >= 0*/shift != 8) {
            decoded += std::to_string((int32_t)ch) + ' ';
            //decoded += ch;
        }

        if(shift >= 8) {
            shift = 0;
        }

        shift += 2;
    }

    return decoded;
}

std::vector<uint8_t> DecodeBase64AsBytes(std::string chars) {
    std::vector<uint8_t> decoded;

    uint32_t shift = 2;

    for(size_t i = 1; i < chars.length(); i++) {
        int32_t first = gBase64Charmap[chars.c_str()[i - 1]];
        int32_t second = gBase64Charmap[chars.c_str()[i]];

        //printf("Iter: %d,\tFirst: %d, Second: %d, F shift L by %d: %d, S shift R by %d: %d, F s + S s: %d\n", i, first, second, shift, ((first << shift) << 24) >> 24, 6 - shift, ((second >> 6 - shift) << 24) >> 24, ((first << shift) + (second >> (6 - shift)) << 24) >> 24);

        unsigned char ch = (((first << shift) + (second >> (6 - shift))) << 24) >> 24;

        if(/*((first << shift) << 24) >> 24 >= 0 && ((second >> 6 - shift) << 24) >> 24 >= 0*/shift != 8) {
            decoded.push_back((uint8_t)ch);
            //decoded += ch;
        }

        if(shift >= 8) {
            shift = 0;
        }

        shift += 2;
    }

    return decoded;
}

template<typename T>
std::vector<T> ConvertBytesTo(std::vector<uint8_t> loadedBytes, uint32_t startOffset = 0, bool littleEndian = true) {
    std::vector<T> result;

    const uint8_t bytes_amount = sizeof(T);
    uint8_t *bytes = new uint8_t[bytes_amount];
    uint32_t byte_counter = 0;

    for(size_t i = startOffset; i < loadedBytes.size(); i++) {
        if(littleEndian) {
            bytes[(bytes_amount - 1) - byte_counter] = loadedBytes[i];
        }
        else {
            bytes[byte_counter] = loadedBytes[i];
        }

        byte_counter++;

        if(byte_counter > (bytes_amount - 1)) {
            byte_counter = 0;

            if(bytes_amount == sizeof(uint8_t)) {
                uint8_t convert = bytes[0];
            
                T value;
                memcpy(&value, &convert, sizeof(T));
                result.push_back(value);
            }
            else if(bytes_amount == sizeof(uint16_t)) {
                uint16_t convert = ((uint16_t)bytes[0] << 8) + bytes[1];
            
                T value;
                memcpy(&value, &convert, sizeof(T));
                result.push_back(value);
            }
            else if(bytes_amount == sizeof(uint32_t)) {
                uint32_t convert = ((uint32_t)bytes[0] << 24) + ((uint32_t)bytes[1] << 16) + ((uint32_t)bytes[2] << 8) + bytes[3];
            
                T value;
                memcpy(&value, &convert, sizeof(T));
                result.push_back(value);
            }
            else if(bytes_amount == sizeof(uint64_t)) {
                uint64_t convert = ((uint64_t)bytes[0] << 56) + ((uint64_t)bytes[1] << 48) + ((uint64_t)bytes[2] << 40) + ((uint64_t)bytes[3] << 32) + ((uint64_t)bytes[4] << 24) + ((uint64_t)bytes[5] << 16) + ((uint64_t)bytes[6] << 8) + bytes[7];

                T value;
                memcpy(&value, &convert, sizeof(T));
                result.push_back(value);
            }
            else {
                uint64_t convert = ((uint64_t)bytes[0] << 56) + ((uint64_t)bytes[1] << 48) + ((uint64_t)bytes[2] << 40) + ((uint64_t)bytes[3] << 32) + ((uint64_t)bytes[4] << 24) + ((uint64_t)bytes[5] << 16) + ((uint64_t)bytes[6] << 8) + bytes[7];

                T value;
                memcpy(&value, &convert, sizeof(T));
                result.push_back(value);
            }
        }
    } 

    delete[] bytes;

    return result;
}

template<typename T>
std::vector<T> ConvertBytesFromFileTo(std::string path, uint32_t startOffset = 0, bool littleEndian = true) {
    std::vector<uint8_t> bytes;
    
    FILE* f = fopen(path.c_str(), "rb+");

    fseek(f, 0, 2);
    uint32_t length = ftell(f);
    fseek(f, 0, 0);

    bytes.resize(length);

    fread(bytes.data(), sizeof(uint8_t), bytes.size(), f);
    fclose(f);

    return ConvertBytesTo<T>(bytes, startOffset, littleEndian);
}

std::string LoadFile(std::string path) {
    std::string contents;

    FILE* f = fopen(path.c_str(), "rb+");

    if(f == nullptr) {
        ENG_ERR("Cannot open file" << path)

        return std::string();
    }

    fseek(f, 0, 2);
    uint32_t length = ftell(f);
    fseek(f, 0, 0);

    contents.resize(length);

    fread(contents.data(), sizeof(contents.data()[0]), contents.size(), f);
    fclose(f);

    return contents;
}

std::vector<uint8_t> LoadBinFile(std::string path, uint32_t startOffset = 0) {
    std::vector<uint8_t> result;
    
    FILE* f = fopen(path.c_str(), "rb+");

    if(f == nullptr) {
        ENG_ERR("Cannot open file " << path)

        return std::vector<uint8_t>();
    }

    fseek(f, 0, 2);
    uint32_t length = ftell(f) - startOffset;
    fseek(f, startOffset, 0);

    result.resize(length);

    fread(result.data(), sizeof(uint8_t), length, f);
    fclose(f);

    return result;
}

#endif