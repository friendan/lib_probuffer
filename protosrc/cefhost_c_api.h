#ifndef CEFHOST_C_API_H
#define CEFHOST_C_API_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// -------------------------- 1.  opaque 类型定义（隐藏内部实现）--------------------------
// 隐藏 Protobuf 生成的 C++ 类，调用方仅通过指针操作
typedef void* CefHost_Param;          // 对应 Protobuf 的 Param 类
typedef void* CefHost_ParamList;      // 对应 Protobuf 的 ParamList 类
typedef void* CefHost_CefRequest;     // 对应 Protobuf 的 CefRequest 类
typedef void* CefHost_CefResponse;    // 对应 Protobuf 的 CefResponse 类

// 参数类型枚举（对应 Protobuf Param 的 oneof 类型）
typedef enum {
    CEFHOST_PARAM_TYPE_NONE = 0,
    CEFHOST_PARAM_TYPE_INT32,
    CEFHOST_PARAM_TYPE_UINT32,
    CEFHOST_PARAM_TYPE_INT64,
    CEFHOST_PARAM_TYPE_UINT64,
    CEFHOST_PARAM_TYPE_STRING,
    CEFHOST_PARAM_TYPE_BOOL,
    CEFHOST_PARAM_TYPE_DOUBLE,
    CEFHOST_PARAM_TYPE_FLOAT,
    CEFHOST_PARAM_TYPE_BYTES,
    CEFHOST_PARAM_TYPE_ARRAY  // 对应 arrayVal（ParamList）
} CefHost_ParamType;

// -------------------------- 2. 内存管理接口（分配/释放）--------------------------
// 导出符号：Windows 下用 __declspec(dllexport)，Linux 下可省略（或用 -fvisibility=default）
#ifdef _WIN32
#define CEFHOST_C_API __declspec(dllexport)
#else
#define CEFHOST_C_API
#endif

// Param 内存管理
CEFHOST_C_API CefHost_Param CefHost_Param_Create();          // 创建 Param 对象
CEFHOST_C_API void CefHost_Param_Destroy(CefHost_Param param); // 释放 Param 对象

// ParamList 内存管理
CEFHOST_C_API CefHost_ParamList CefHost_ParamList_Create();  // 创建 ParamList 对象
CEFHOST_C_API void CefHost_ParamList_Destroy(CefHost_ParamList list); // 释放 ParamList 对象

// CefRequest/CefResponse 内存管理
CEFHOST_C_API CefHost_CefRequest CefHost_CefRequest_Create(); // 创建 CefRequest 对象
CEFHOST_C_API void CefHost_CefRequest_Destroy(CefHost_CefRequest req); // 释放 CefRequest

CEFHOST_C_API CefHost_CefResponse CefHost_CefResponse_Create(); // 创建 CefResponse 对象
CEFHOST_C_API void CefHost_CefResponse_Destroy(CefHost_CefResponse resp); // 释放 CefResponse

// -------------------------- 3. Param 操作接口（设置/获取值）--------------------------
// 设置 Param 的值（根据类型选择对应函数）
CEFHOST_C_API void CefHost_Param_SetInt32(CefHost_Param param, int32_t value);
CEFHOST_C_API void CefHost_Param_SetUInt32(CefHost_Param param, uint32_t value);
CEFHOST_C_API void CefHost_Param_SetInt64(CefHost_Param param, int64_t value);
CEFHOST_C_API void CefHost_Param_SetUInt64(CefHost_Param param, uint64_t value);
CEFHOST_C_API void CefHost_Param_SetString(CefHost_Param param, const char* value); // value 需是 UTF-8 字符串
CEFHOST_C_API void CefHost_Param_SetBool(CefHost_Param param, bool value);
CEFHOST_C_API void CefHost_Param_SetDouble(CefHost_Param param, double value);
CEFHOST_C_API void CefHost_Param_SetFloat(CefHost_Param param, float value);
CEFHOST_C_API void CefHost_Param_SetBytes(CefHost_Param param, const uint8_t* bytes, size_t len); // 二进制数据
CEFHOST_C_API void CefHost_Param_SetArray(CefHost_Param param, CefHost_ParamList list); // 设置数组（ParamList）

// 获取 Param 的值（先通过 CefHost_Param_GetType 获取类型，再调用对应函数）
CEFHOST_C_API CefHost_ParamType CefHost_Param_GetType(const CefHost_Param param);
CEFHOST_C_API int32_t CefHost_Param_GetInt32(const CefHost_Param param);
CEFHOST_C_API uint32_t CefHost_Param_GetUInt32(const CefHost_Param param);
CEFHOST_C_API int64_t CefHost_Param_GetInt64(const CefHost_Param param);
CEFHOST_C_API uint64_t CefHost_Param_GetUInt64(const CefHost_Param param);
CEFHOST_C_API const char* CefHost_Param_GetString(const CefHost_Param param); // 返回内部指针，无需调用方释放
CEFHOST_C_API bool CefHost_Param_GetBool(const CefHost_Param param);
CEFHOST_C_API double CefHost_Param_GetDouble(const CefHost_Param param);
CEFHOST_C_API float CefHost_Param_GetFloat(const CefHost_Param param);
// 获取二进制数据：返回数据长度，数据通过 out_bytes 输出（内部指针，无需释放）
CEFHOST_C_API size_t CefHost_Param_GetBytes(const CefHost_Param param, const uint8_t** out_bytes);
CEFHOST_C_API CefHost_ParamList CefHost_Param_GetArray(const CefHost_Param param); // 返回的 array 需调用方手动释放

// -------------------------- 4. ParamList 操作接口（数组操作）--------------------------
CEFHOST_C_API void CefHost_ParamList_AddParam(CefHost_ParamList list, CefHost_Param param); // 添加 Param 到数组（param 所有权转移，无需调用方释放）
CEFHOST_C_API size_t CefHost_ParamList_GetCount(const CefHost_ParamList list); // 获取数组元素个数
CEFHOST_C_API CefHost_Param CefHost_ParamList_GetParam(const CefHost_ParamList list, size_t index); // 获取指定索引的 Param（返回的 param 需调用方释放）

// -------------------------- 5. CefRequest/CefResponse 操作接口--------------------------
// CefRequest：添加参数到 paramList / paramMap
CEFHOST_C_API void CefHost_CefRequest_AddParamToList(CefHost_CefRequest req, CefHost_Param param); // param 所有权转移
CEFHOST_C_API void CefHost_CefRequest_AddParamToMap(CefHost_CefRequest req, const char* key, CefHost_Param param); // key 是 UTF-8 字符串，param 所有权转移

// CefResponse：添加参数到 paramList / paramMap（与 CefRequest 类似）
CEFHOST_C_API void CefHost_CefResponse_AddParamToList(CefHost_CefResponse resp, CefHost_Param param);
CEFHOST_C_API void CefHost_CefResponse_AddParamToMap(CefHost_CefResponse resp, const char* key, CefHost_Param param);

// -------------------------- 6. 序列化/反序列化接口（Protobuf 核心功能封装）--------------------------
// 序列化：将 CefRequest/CefResponse 转为二进制数据（返回数据长度，out_data 需调用方用 CefHost_FreeSerializedData 释放）
CEFHOST_C_API size_t CefHost_CefRequest_Serialize(const CefHost_CefRequest req, uint8_t** out_data);
CEFHOST_C_API size_t CefHost_CefResponse_Serialize(const CefHost_CefResponse resp, uint8_t** out_data);

// 反序列化：将二进制数据转为 CefRequest/CefResponse（失败返回 NULL）
CEFHOST_C_API CefHost_CefRequest CefHost_CefRequest_Deserialize(const uint8_t* data, size_t len);
CEFHOST_C_API CefHost_CefResponse CefHost_CefResponse_Deserialize(const uint8_t* data, size_t len);

// 释放序列化生成的二进制数据
CEFHOST_C_API void CefHost_FreeSerializedData(uint8_t* data);

#endif // CEFHOST_C_API_H