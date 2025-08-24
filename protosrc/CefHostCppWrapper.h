#ifndef CEFHOST_CPP_WRAPPER_H
#define CEFHOST_CPP_WRAPPER_H

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>
#include <mutex>
#include <call_once>
#include <stdexcept>

// 1. 引入原始C接口头文件（请替换为实际路径）
#include "cefhost_c_api.h"

// 2. 跨平台动态加载API
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

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

    // -------------------------- DLL加载器（单例） --------------------------
    class DllLoader {
    public:
        static DllLoader& GetInstance() {
            static DllLoader s_instance;
            return s_instance;
        }

        void Init(const std::string& dll_path) {
            std::call_once(m_init_flag, [this, &dll_path]() {
#ifdef _WIN32
                m_dll_handle = LoadLibraryA(dll_path.c_str());
#else
                m_dll_handle = dlopen(dll_path.c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif

                if (!m_dll_handle) {
                    std::string err_msg;
#ifdef _WIN32
                    char win_err[256] = { 0 };
                    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, win_err, sizeof(win_err), NULL);
                    err_msg = win_err;
#else
                    err_msg = dlerror() ? dlerror() : "Unknown error";
#endif
                    throw std::runtime_error("DLL load failed: " + dll_path + " | " + err_msg);
                }
                });
        }

        bool IsLoaded() const { return m_dll_handle != nullptr; }

        template <typename FuncPtr>
        FuncPtr GetFunction(const std::string& func_name) const {
            if (!m_dll_handle) {
                throw std::runtime_error("DLL not initialized! Call DllLoader::Init first.");
            }

            void* func_addr = nullptr;
#ifdef _WIN32
            func_addr = GetProcAddress(static_cast<HMODULE>(m_dll_handle), func_name.c_str());
#else
            func_addr = dlsym(m_dll_handle, func_name.c_str());
#endif

            if (!func_addr) {
                std::string err_msg;
#ifdef _WIN32
                char win_err[256] = { 0 };
                FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, win_err, sizeof(win_err), NULL);
                err_msg = win_err;
#else
                err_msg = dlerror() ? dlerror() : "Unknown error";
#endif
                throw std::runtime_error("Get function failed: " + func_name + " | " + err_msg);
            }

            return reinterpret_cast<FuncPtr>(func_addr);
        }

        CEFHOST_DISABLE_COPY_MOVE(DllLoader)

    private:
        DllLoader() = default;
        ~DllLoader() {
            if (m_dll_handle) {
#ifdef _WIN32
                FreeLibrary(static_cast<HMODULE>(m_dll_handle));
#else
                dlclose(m_dll_handle);
#endif
                m_dll_handle = nullptr;
            }
        }

    private:
        void* m_dll_handle = nullptr;
        std::once_flag m_init_flag;
    };

    // 前向声明
    class ParamList;

    // -------------------------- 序列化数据封装 --------------------------
    class SerializedData {
    public:
        SerializedData(uint8_t* data = nullptr, size_t len = 0)
            : m_data(data), m_len(len) {}

        ~SerializedData() {
            if (m_data && Param::s_func_FreeSerializedData) {
                Param::s_func_FreeSerializedData(m_data);
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
        static void Init() {
            std::call_once(s_init_flag, []() {
                auto& loader = DllLoader::GetInstance();
                if (!loader.IsLoaded()) {
                    throw std::runtime_error("Param::Init failed: DLL not loaded.");
                }

                s_func_Create = loader.GetFunction<decltype(s_func_Create)>("CefHost_Param_Create");
                s_func_Destroy = loader.GetFunction<decltype(s_func_Destroy)>("CefHost_Param_Destroy");
                s_func_SetInt32 = loader.GetFunction<decltype(s_func_SetInt32)>("CefHost_Param_SetInt32");
                s_func_SetUInt32 = loader.GetFunction<decltype(s_func_SetUInt32)>("CefHost_Param_SetUInt32");
                s_func_SetInt64 = loader.GetFunction<decltype(s_func_SetInt64)>("CefHost_Param_SetInt64");
                s_func_SetUInt64 = loader.GetFunction<decltype(s_func_SetUInt64)>("CefHost_Param_SetUInt64");
                s_func_SetString = loader.GetFunction<decltype(s_func_SetString)>("CefHost_Param_SetString");
                s_func_SetBool = loader.GetFunction<decltype(s_func_SetBool)>("CefHost_Param_SetBool");
                s_func_SetDouble = loader.GetFunction<decltype(s_func_SetDouble)>("CefHost_Param_SetDouble");
                s_func_SetFloat = loader.GetFunction<decltype(s_func_SetFloat)>("CefHost_Param_SetFloat");
                s_func_SetBytes = loader.GetFunction<decltype(s_func_SetBytes)>("CefHost_Param_SetBytes");
                s_func_SetArray = loader.GetFunction<decltype(s_func_SetArray)>("CefHost_Param_SetArray");
                s_func_GetType = loader.GetFunction<decltype(s_func_GetType)>("CefHost_Param_GetType");
                s_func_GetInt32 = loader.GetFunction<decltype(s_func_GetInt32)>("CefHost_Param_GetInt32");
                s_func_GetUInt32 = loader.GetFunction<decltype(s_func_GetUInt32)>("CefHost_Param_GetUInt32");
                s_func_GetInt64 = loader.GetFunction<decltype(s_func_GetInt64)>("CefHost_Param_GetInt64");
                s_func_GetUInt64 = loader.GetFunction<decltype(s_func_GetUInt64)>("CefHost_Param_GetUInt64");
                s_func_GetString = loader.GetFunction<decltype(s_func_GetString)>("CefHost_Param_GetString");
                s_func_GetBool = loader.GetFunction<decltype(s_func_GetBool)>("CefHost_Param_GetBool");
                s_func_GetDouble = loader.GetFunction<decltype(s_func_GetDouble)>("CefHost_Param_GetDouble");
                s_func_GetFloat = loader.GetFunction<decltype(s_func_GetFloat)>("CefHost_Param_GetFloat");
                s_func_GetBytes = loader.GetFunction<decltype(s_func_GetBytes)>("CefHost_Param_GetBytes");
                s_func_GetArray = loader.GetFunction<decltype(s_func_GetArray)>("CefHost_Param_GetArray");
                s_func_FreeSerializedData = loader.GetFunction<decltype(s_func_FreeSerializedData)>("CefHost_FreeSerializedData");
                });
        }

        Param() {
            if (!s_func_Create) throw std::runtime_error("Param not initialized! Call Param::Init.");
            m_handle = s_func_Create();
            if (!m_handle) throw std::runtime_error("Param create failed.");
        }

        ~Param() {
            if (m_handle && s_func_Destroy) {
                s_func_Destroy(m_handle);
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
        Param& SetInt32(int32_t value) { s_func_SetInt32(m_handle, value); return *this; }
        Param& SetUInt32(uint32_t value) { s_func_SetUInt32(m_handle, value); return *this; }
        Param& SetInt64(int64_t value) { s_func_SetInt64(m_handle, value); return *this; }
        Param& SetUInt64(uint64_t value) { s_func_SetUInt64(m_handle, value); return *this; }
        Param& SetString(const char* value) { s_func_SetString(m_handle, value); return *this; }
        Param& SetBool(bool value) { s_func_SetBool(m_handle, value); return *this; }
        Param& SetDouble(double value) { s_func_SetDouble(m_handle, value); return *this; }
        Param& SetFloat(float value) { s_func_SetFloat(m_handle, value); return *this; }
        Param& SetBytes(const uint8_t* bytes, size_t len) { s_func_SetBytes(m_handle, bytes, len); return *this; }
        Param& SetArray(const ParamList& list);

        // 获取值
        CefHost_ParamType GetType() const { return s_func_GetType(m_handle); }
        int32_t GetInt32() const { return s_func_GetInt32(m_handle); }
        uint32_t GetUInt32() const { return s_func_GetUInt32(m_handle); }
        int64_t GetInt64() const { return s_func_GetInt64(m_handle); }
        uint64_t GetUInt64() const { return s_func_GetUInt64(m_handle); }
        const char* GetString() const { return s_func_GetString(m_handle); }
        bool GetBool() const { return s_func_GetBool(m_handle); }
        double GetDouble() const { return s_func_GetDouble(m_handle); }
        float GetFloat() const { return s_func_GetFloat(m_handle); }
        std::pair<const uint8_t*, size_t> GetBytes() const {
            const uint8_t* bytes = nullptr;
            size_t len = s_func_GetBytes(m_handle, &bytes);
            return { bytes, len };
        }
        ParamList GetArray() const;

        CefHost_Param GetRawHandle() const { return m_handle; }

    private:
        // 声明 SerializedData 为友元类，允许其访问私有成员
        friend class SerializedData;
        CefHost_Param m_handle = nullptr;

        // 静态函数指针声明
        static decltype(&CefHost_Param_Create) s_func_Create;
        static decltype(&CefHost_Param_Destroy) s_func_Destroy;
        static decltype(&CefHost_Param_SetInt32) s_func_SetInt32;
        static decltype(&CefHost_Param_SetUInt32) s_func_SetUInt32;
        static decltype(&CefHost_Param_SetInt64) s_func_SetInt64;
        static decltype(&CefHost_Param_SetUInt64) s_func_SetUInt64;
        static decltype(&CefHost_Param_SetString) s_func_SetString;
        static decltype(&CefHost_Param_SetBool) s_func_SetBool;
        static decltype(&CefHost_Param_SetDouble) s_func_SetDouble;
        static decltype(&CefHost_Param_SetFloat) s_func_SetFloat;
        static decltype(&CefHost_Param_SetBytes) s_func_SetBytes;
        static decltype(&CefHost_Param_SetArray) s_func_SetArray;
        static decltype(&CefHost_Param_GetType) s_func_GetType;
        static decltype(&CefHost_Param_GetInt32) s_func_GetInt32;
        static decltype(&CefHost_Param_GetUInt32) s_func_GetUInt32;
        static decltype(&CefHost_Param_GetInt64) s_func_GetInt64;
        static decltype(&CefHost_Param_GetUInt64) s_func_GetUInt64;
        static decltype(&CefHost_Param_GetString) s_func_GetString;
        static decltype(&CefHost_Param_GetBool) s_func_GetBool;
        static decltype(&CefHost_Param_GetDouble) s_func_GetDouble;
        static decltype(&CefHost_Param_GetFloat) s_func_GetFloat;
        static decltype(&CefHost_Param_GetBytes) s_func_GetBytes;
        static decltype(&CefHost_Param_GetArray) s_func_GetArray;
        static decltype(&CefHost_FreeSerializedData) s_func_FreeSerializedData;

        static std::once_flag s_init_flag;
    };

    // -------------------------- ParamList封装 --------------------------
    class ParamList {
    public:
        static void Init() {
            std::call_once(s_init_flag, []() {
                auto& loader = DllLoader::GetInstance();
                if (!loader.IsLoaded()) {
                    throw std::runtime_error("ParamList::Init failed: DLL not loaded.");
                }

                s_func_Create = loader.GetFunction<decltype(s_func_Create)>("CefHost_ParamList_Create");
                s_func_Destroy = loader.GetFunction<decltype(s_func_Destroy)>("CefHost_ParamList_Destroy");
                s_func_AddParam = loader.GetFunction<decltype(s_func_AddParam)>("CefHost_ParamList_AddParam");
                s_func_GetCount = loader.GetFunction<decltype(s_func_GetCount)>("CefHost_ParamList_GetCount");
                s_func_GetParam = loader.GetFunction<decltype(s_func_GetParam)>("CefHost_ParamList_GetParam");
                });
        }

        ParamList() {
            if (!s_func_Create) throw std::runtime_error("ParamList not initialized! Call ParamList::Init.");
            m_handle = s_func_Create();
            if (!m_handle) throw std::runtime_error("ParamList create failed.");
        }

        ~ParamList() {
            if (m_handle && s_func_Destroy) {
                s_func_Destroy(m_handle);
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
            s_func_AddParam(m_handle, param.GetRawHandle());
            param.m_handle = nullptr; // 转移所有权
        }

        size_t GetCount() const { return s_func_GetCount(m_handle); }

        Param GetParam(size_t index) const {
            if (index >= GetCount()) {
                throw std::out_of_range("ParamList index out of range.");
            }
            CefHost_Param param_handle = s_func_GetParam(m_handle, index);
            if (!param_handle) {
                throw std::runtime_error("Failed to get Param from ParamList.");
            }
            // 构造Param并转移句柄所有权
            Param param;
            param.m_handle = param_handle;
            return param;
        }

        CefHost_ParamList GetRawHandle() const { return m_handle; }

    private:
        // 声明 SerializedData 为友元类，允许其访问私有成员
        friend class SerializedData;
        CefHost_ParamList m_handle = nullptr;

        // 静态函数指针声明
        static decltype(&CefHost_ParamList_Create) s_func_Create;
        static decltype(&CefHost_ParamList_Destroy) s_func_Destroy;
        static decltype(&CefHost_ParamList_AddParam) s_func_AddParam;
        static decltype(&CefHost_ParamList_GetCount) s_func_GetCount;
        static decltype(&CefHost_ParamList_GetParam) s_func_GetParam;

        static std::once_flag s_init_flag;

        // 允许Param访问私有成员
        friend class Param;
    };

    // -------------------------- 循环依赖接口实现 --------------------------
    inline Param& Param::SetArray(const ParamList& list) {
        s_func_SetArray(m_handle, list.m_handle);
        return *this;
    }

    inline ParamList Param::GetArray() const {
        CefHost_ParamList list_handle = s_func_GetArray(m_handle);
        if (!list_handle) {
            throw std::runtime_error("Failed to get ParamList from Param.");
        }
        ParamList list;
        list.m_handle = list_handle;
        return list;
    }

    // -------------------------- 静态成员变量定义（关键！必须有） --------------------------
    // Param类静态成员定义
    decltype(&CefHost_Param_Create) Param::s_func_Create = nullptr;
    decltype(&CefHost_Param_Destroy) Param::s_func_Destroy = nullptr;
    decltype(&CefHost_Param_SetInt32) Param::s_func_SetInt32 = nullptr;
    decltype(&CefHost_Param_SetUInt32) Param::s_func_SetUInt32 = nullptr;
    decltype(&CefHost_Param_SetInt64) Param::s_func_SetInt64 = nullptr;
    decltype(&CefHost_Param_SetUInt64) Param::s_func_SetUInt64 = nullptr;
    decltype(&CefHost_Param_SetString) Param::s_func_SetString = nullptr;
    decltype(&CefHost_Param_SetBool) Param::s_func_SetBool = nullptr;
    decltype(&CefHost_Param_SetDouble) Param::s_func_SetDouble = nullptr;
    decltype(&CefHost_Param_SetFloat) Param::s_func_SetFloat = nullptr;
    decltype(&CefHost_Param_SetBytes) Param::s_func_SetBytes = nullptr;
    decltype(&CefHost_Param_SetArray) Param::s_func_SetArray = nullptr;
    decltype(&CefHost_Param_GetType) Param::s_func_GetType = nullptr;
    decltype(&CefHost_Param_GetInt32) Param::s_func_GetInt32 = nullptr;
    decltype(&CefHost_Param_GetUInt32) Param::s_func_GetUInt32 = nullptr;
    decltype(&CefHost_Param_GetInt64) Param::s_func_GetInt64 = nullptr;
    decltype(&CefHost_Param_GetUInt64) Param::s_func_GetUInt64 = nullptr;
    decltype(&CefHost_Param_GetString) Param::s_func_GetString = nullptr;
    decltype(&CefHost_Param_GetBool) Param::s_func_GetBool = nullptr;
    decltype(&CefHost_Param_GetDouble) Param::s_func_GetDouble = nullptr;
    decltype(&CefHost_Param_GetFloat) Param::s_func_GetFloat = nullptr;
    decltype(&CefHost_Param_GetBytes) Param::s_func_GetBytes = nullptr;
    decltype(&CefHost_Param_GetArray) Param::s_func_GetArray = nullptr;
    decltype(&CefHost_FreeSerializedData) Param::s_func_FreeSerializedData = nullptr;
    std::once_flag Param::s_init_flag;

    // ParamList类静态成员定义
    decltype(&CefHost_ParamList_Create) ParamList::s_func_Create = nullptr;
    decltype(&CefHost_ParamList_Destroy) ParamList::s_func_Destroy = nullptr;
    decltype(&CefHost_ParamList_AddParam) ParamList::s_func_AddParam = nullptr;
    decltype(&CefHost_ParamList_GetCount) ParamList::s_func_GetCount = nullptr;
    decltype(&CefHost_ParamList_GetParam) ParamList::s_func_GetParam = nullptr;
    std::once_flag ParamList::s_init_flag;

} // namespace CefHost

#endif // CEFHOST_CPP_WRAPPER_H
