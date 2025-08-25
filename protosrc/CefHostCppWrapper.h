#ifndef CEFHOST_CPP_WRAPPER_H
#define CEFHOST_CPP_WRAPPER_H
#include <windows.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>
#include <mutex>
//#include <call_once>
#include <stdexcept>
extern "C" {
    #include "cefhost_c_api.h"
}



// 3. 工具宏
#define CEFHOST_DISABLE_COPY(Class) \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete;

#define CEFHOST_DISABLE_MOVE(Class) \
    Class(Class&&) = delete; \
    Class& operator=(Class&&) = delete;

#define CEFHOST_DISABLE_COPY_MOVE(Class) \
    CEFHOST_DISABLE_COPY(Class); \
    CEFHOST_DISABLE_MOVE(Class);

namespace CefHost {
    class ParamList;

    class CApi {
    public:
        static CApi& GetInstance() {
            static CApi s_instance;
            return s_instance;
        }

        template <typename FuncPtr>
        FuncPtr GetFunction(const std::string& func_name) const {
            void* func_addr = nullptr;
            func_addr = GetProcAddress(static_cast<HMODULE>(mDLL), func_name.c_str());
            return reinterpret_cast<FuncPtr>(func_addr);
        }

        bool InitApi() {
            if (mInit) return true;
            mDLL = LoadLibraryA("probuffer.dll");
            if (mDLL == NULL) return false;

            CefHost_Param_Create = GetFunction<decltype(CefHost_Param_Create)>("CefHost_Param_Create");
            CefHost_Param_Destroy = GetFunction<decltype(CefHost_Param_Destroy)>("CefHost_Param_Destroy");
            CefHost_Param_SetInt32 = GetFunction<decltype(CefHost_Param_SetInt32)>("CefHost_Param_SetInt32");
            CefHost_Param_SetUInt32 = GetFunction<decltype(CefHost_Param_SetUInt32)>("CefHost_Param_SetUInt32");
            CefHost_Param_SetInt64 = GetFunction<decltype(CefHost_Param_SetInt64)>("CefHost_Param_SetInt64");
            CefHost_Param_SetUInt64 = GetFunction<decltype(CefHost_Param_SetUInt64)>("CefHost_Param_SetUInt64");
            CefHost_Param_SetString = GetFunction<decltype(CefHost_Param_SetString)>("CefHost_Param_SetString");
            CefHost_Param_SetBool = GetFunction<decltype(CefHost_Param_SetBool)>("CefHost_Param_SetBool");
            CefHost_Param_SetDouble = GetFunction<decltype(CefHost_Param_SetDouble)>("CefHost_Param_SetDouble");
            CefHost_Param_SetFloat = GetFunction<decltype(CefHost_Param_SetFloat)>("CefHost_Param_SetFloat");
            CefHost_Param_SetBytes = GetFunction<decltype(CefHost_Param_SetBytes)>("CefHost_Param_SetBytes");
            CefHost_Param_SetArray = GetFunction<decltype(CefHost_Param_SetArray)>("CefHost_Param_SetArray");
			
            CefHost_Param_GetType = GetFunction<decltype(CefHost_Param_GetType)>("CefHost_Param_GetType");
            CefHost_Param_GetInt32 = GetFunction<decltype(CefHost_Param_GetInt32)>("CefHost_Param_GetInt32");
            CefHost_Param_GetUInt32 = GetFunction<decltype(CefHost_Param_GetUInt32)>("CefHost_Param_GetUInt32");
            CefHost_Param_GetInt64 = GetFunction<decltype(CefHost_Param_GetInt64)>("CefHost_Param_GetInt64");
            CefHost_Param_GetUInt64 = GetFunction<decltype(CefHost_Param_GetUInt64)>("CefHost_Param_GetUInt64");
            CefHost_Param_GetString = GetFunction<decltype(CefHost_Param_GetString)>("CefHost_Param_GetString");
            CefHost_Param_GetBool = GetFunction<decltype(CefHost_Param_GetBool)>("CefHost_Param_GetBool");
            CefHost_Param_GetDouble = GetFunction<decltype(CefHost_Param_GetDouble)>("CefHost_Param_GetDouble");
            CefHost_Param_GetFloat = GetFunction<decltype(CefHost_Param_GetFloat)>("CefHost_Param_GetFloat");
            CefHost_Param_GetBytes = GetFunction<decltype(CefHost_Param_GetBytes)>("CefHost_Param_GetBytes");
            CefHost_Param_GetArray = GetFunction<decltype(CefHost_Param_GetArray)>("CefHost_Param_GetArray");

            CefHost_ParamList_Create = GetFunction<decltype(CefHost_ParamList_Create)>("CefHost_ParamList_Create");
            CefHost_ParamList_Destroy = GetFunction<decltype(CefHost_ParamList_Destroy)>("CefHost_ParamList_Destroy");
            CefHost_ParamList_AddParam = GetFunction<decltype(CefHost_ParamList_AddParam)>("CefHost_ParamList_AddParam");
            CefHost_ParamList_GetCount = GetFunction<decltype(CefHost_ParamList_GetCount)>("CefHost_ParamList_GetCount");
            CefHost_ParamList_GetParam = GetFunction<decltype(CefHost_ParamList_GetParam)>("CefHost_ParamList_GetParam");

            CefHost_FreeSerializedData = GetFunction<decltype(CefHost_FreeSerializedData)>("CefHost_FreeSerializedData");

            mInit = true;
            return true;
        }

        // 静态函数指针声明
        static decltype(&CefHost_Param_Create) CefHost_Param_Create;
        static decltype(&CefHost_Param_Destroy) CefHost_Param_Destroy;
        static decltype(&CefHost_Param_SetInt32) CefHost_Param_SetInt32;
        static decltype(&CefHost_Param_SetUInt32) CefHost_Param_SetUInt32;
        static decltype(&CefHost_Param_SetInt64) CefHost_Param_SetInt64;
        static decltype(&CefHost_Param_SetUInt64) CefHost_Param_SetUInt64;
        static decltype(&CefHost_Param_SetString) CefHost_Param_SetString;
        static decltype(&CefHost_Param_SetBool) CefHost_Param_SetBool;
        static decltype(&CefHost_Param_SetDouble) CefHost_Param_SetDouble;
        static decltype(&CefHost_Param_SetFloat) CefHost_Param_SetFloat;
        static decltype(&CefHost_Param_SetBytes) CefHost_Param_SetBytes;
        static decltype(&CefHost_Param_SetArray) CefHost_Param_SetArray;
        static decltype(&CefHost_Param_GetType) CefHost_Param_GetType;
        static decltype(&CefHost_Param_GetInt32) CefHost_Param_GetInt32;
        static decltype(&CefHost_Param_GetUInt32) CefHost_Param_GetUInt32;
        static decltype(&CefHost_Param_GetInt64) CefHost_Param_GetInt64;
        static decltype(&CefHost_Param_GetUInt64) CefHost_Param_GetUInt64;
        static decltype(&CefHost_Param_GetString) CefHost_Param_GetString;
        static decltype(&CefHost_Param_GetBool) CefHost_Param_GetBool;
        static decltype(&CefHost_Param_GetDouble) CefHost_Param_GetDouble;
        static decltype(&CefHost_Param_GetFloat) CefHost_Param_GetFloat;
        static decltype(&CefHost_Param_GetBytes) CefHost_Param_GetBytes;
        static decltype(&CefHost_Param_GetArray) CefHost_Param_GetArray;

        static decltype(&CefHost_FreeSerializedData) CefHost_FreeSerializedData;

        static decltype(&CefHost_ParamList_Create) CefHost_ParamList_Create;
        static decltype(&CefHost_ParamList_Destroy) CefHost_ParamList_Destroy;
        static decltype(&CefHost_ParamList_AddParam) CefHost_ParamList_AddParam;
        static decltype(&CefHost_ParamList_GetCount) CefHost_ParamList_GetCount;
        static decltype(&CefHost_ParamList_GetParam) CefHost_ParamList_GetParam;

    private:
        CApi() :
            mInit(false),
            mDLL(NULL)
        {

        }
        bool mInit;
        HMODULE mDLL;
    };

    // -------------------------- 序列化数据封装 --------------------------
    class SerializedData {
    public:
        SerializedData(uint8_t* data = nullptr, size_t len = 0)
            : m_data(data), m_len(len) {}

        ~SerializedData() {
            if (m_data) {
                CApi::CefHost_FreeSerializedData(m_data);
                m_data = nullptr;
            }
        }

        CEFHOST_DISABLE_COPY(SerializedData)
            SerializedData(SerializedData&& other) noexcept
            : m_data(other.m_data), m_len(other.m_len) {
            other.m_data = nullptr;
            other.m_len = 0;
        }
        SerializedData& operator=(SerializedData&& other) noexcept {
            if (this != &other) {
                m_data = other.m_data;
                m_len = other.m_len;
                other.m_data = nullptr;
                other.m_len = 0;
            }
            return *this;
        }

        const uint8_t* GetData() const { return m_data; }
        size_t GetLength() const { return m_len; }
        bool IsValid() const { return m_data != nullptr && m_len > 0; }

    private:
        uint8_t* m_data = nullptr;
        size_t m_len = 0;

        friend class CefRequest;
        friend class CefResponse;
    };

    // -------------------------- Param封装 --------------------------
    class Param {
    public:
        
        Param() {
            m_handle = CApi::CefHost_Param_Create();
        }

        ~Param() {
            if (m_handle) {
                CApi::CefHost_Param_Destroy(m_handle);
                m_handle = nullptr;
            }
        }

        CEFHOST_DISABLE_COPY(Param)
            Param(Param&& other) noexcept : m_handle(other.m_handle) { other.m_handle = nullptr; }
        Param& operator=(Param&& other) noexcept {
            if (this != &other) { m_handle = other.m_handle; other.m_handle = nullptr; }
            return *this;
        }

        // 设置值
        Param& SetInt32(int32_t value) { CApi::CefHost_Param_SetInt32(m_handle, value); return *this; }
        Param& SetUInt32(uint32_t value) { CApi::CefHost_Param_SetUInt32(m_handle, value); return *this; }
        Param& SetInt64(int64_t value) { CApi::CefHost_Param_SetInt64(m_handle, value); return *this; }
        Param& SetUInt64(uint64_t value) { CApi::CefHost_Param_SetUInt64(m_handle, value); return *this; }
        Param& SetString(const char* value) { CApi::CefHost_Param_SetString(m_handle, value); return *this; }
        Param& SetBool(bool value) { CApi::CefHost_Param_SetBool(m_handle, value); return *this; }
        Param& SetDouble(double value) { CApi::CefHost_Param_SetDouble(m_handle, value); return *this; }
        Param& SetFloat(float value) { CApi::CefHost_Param_SetFloat(m_handle, value); return *this; }
        Param& SetBytes(const uint8_t* bytes, size_t len) { CApi::CefHost_Param_SetBytes(m_handle, bytes, len); return *this; }

        // -------------------------- 循环依赖接口实现 --------------------------
        inline Param& SetArray(const ParamList& list);

        // 获取值
        CefHost_ParamType GetType() const { return CApi::CefHost_Param_GetType(m_handle); }
        int32_t GetInt32() const { return CApi::CefHost_Param_GetInt32(m_handle); }
        uint32_t GetUInt32() const { return CApi::CefHost_Param_GetUInt32(m_handle); }
        int64_t GetInt64() const { return CApi::CefHost_Param_GetInt64(m_handle); }
        uint64_t GetUInt64() const { return CApi::CefHost_Param_GetUInt64(m_handle); }
        const char* GetString() const { return CApi::CefHost_Param_GetString(m_handle); }
        bool GetBool() const { return CApi::CefHost_Param_GetBool(m_handle); }
        double GetDouble() const { return CApi::CefHost_Param_GetDouble(m_handle); }
        float GetFloat() const { return CApi::CefHost_Param_GetFloat(m_handle); }

        std::pair<const uint8_t*, size_t> GetBytes() const {
            const uint8_t* bytes = nullptr;
            size_t len = CApi::CefHost_Param_GetBytes(m_handle, &bytes);
            return { bytes, len };
        }

        inline ParamList GetArray() const;

        CefHost_Param GetRawHandle() const { return m_handle; }

    private:
        // 声明 SerializedData 为友元类，允许其访问私有成员
        friend class SerializedData;
        friend class ParamList;
        CefHost_Param m_handle = nullptr;
    };

    // -------------------------- ParamList封装 --------------------------
    class ParamList {
    public:
 

        ParamList() {
            m_handle = CApi::CefHost_ParamList_Create();
        }

        ~ParamList() {
            if (m_handle) {
                CApi::CefHost_ParamList_Destroy(m_handle);
                m_handle = nullptr;
            }
        }

        CEFHOST_DISABLE_COPY(ParamList)
            ParamList(ParamList&& other) noexcept : m_handle(other.m_handle) { other.m_handle = nullptr; }
        ParamList& operator=(ParamList&& other) noexcept {
            if (this != &other) { m_handle = other.m_handle; other.m_handle = nullptr; }
            return *this;
        }

        // 数组操作
        void AddParam(Param param) {
            CApi::CefHost_ParamList_AddParam(m_handle, param.GetRawHandle());
            param.m_handle = nullptr; // 转移所有权
        }

        size_t GetCount() const { return CApi::CefHost_ParamList_GetCount(m_handle); }

        Param GetParam(size_t index) const {
            if (index >= GetCount()) {
                throw std::out_of_range("ParamList index out of range.");
            }
            CefHost_Param param_handle = CApi::CefHost_ParamList_GetParam(m_handle, index);
            if (!param_handle) {
                throw std::runtime_error("Failed to get Param from ParamList.");
            }
            // 构造Param并转移句柄所有权
            Param param;
            param.m_handle = param_handle;
            return param;
        }

        CefHost_ParamList GetRawHandle() const { return m_handle; }
        CefHost_ParamList m_handle = nullptr;
    };

    inline Param& Param::SetArray(const ParamList& list) {
        CApi::CefHost_Param_SetArray(m_handle, list.m_handle);
        return *this;
    }

    inline ParamList Param::GetArray() const {
        CefHost_ParamList list_handle = CApi::CefHost_Param_GetArray(m_handle);
        if (!list_handle) {
            throw std::runtime_error("Failed to get ParamList from Param.");
        }
        ParamList list;
        list.m_handle = list_handle;
        return list;
    }

} // namespace CefHost

#endif // CEFHOST_CPP_WRAPPER_H
