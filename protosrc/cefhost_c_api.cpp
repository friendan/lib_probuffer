#include "cefhost_c_api.h"
#include "cefhost.pb.h"  // 依赖Protobuf生成的C++头文件
#include <cstring>
#include <new>
#include <stdexcept>

// 类型转换宏（内部使用，确保指针安全转换）
#define TO_PARAM(param)      (reinterpret_cast<cefhost::Param*>(param))
#define TO_PARAM_LIST(list)  (reinterpret_cast<cefhost::ParamList*>(list))
#define TO_REQUEST(req)      (reinterpret_cast<cefhost::CefRequest*>(req))
#define TO_RESPONSE(resp)    (reinterpret_cast<cefhost::CefResponse*>(resp))

#define TO_CONST_PARAM(param)      (reinterpret_cast<const cefhost::Param*>(param))
#define TO_CONST_PARAM_LIST(list)  (reinterpret_cast<const cefhost::ParamList*>(list))
#define TO_CONST_REQUEST(req)      (reinterpret_cast<const cefhost::CefRequest*>(req))
#define TO_CONST_RESPONSE(resp)    (reinterpret_cast<const cefhost::CefResponse*>(resp))

// -------------------------- Param 内存管理 --------------------------
CEFHOST_C_API CefHost_Param CefHost_Param_Create() {
    try {
        return new cefhost::Param();
    }
    catch (...) {
        return nullptr;
    }
}

CEFHOST_C_API void CefHost_Param_Destroy(CefHost_Param param) {
    if (param != nullptr) {
        delete TO_PARAM(param);
    }
}

// -------------------------- ParamList 内存管理 --------------------------
CEFHOST_C_API CefHost_ParamList CefHost_ParamList_Create() {
    try {
        return new cefhost::ParamList();
    }
    catch (...) {
        return nullptr;
    }
}

CEFHOST_C_API void CefHost_ParamList_Destroy(CefHost_ParamList list) {
    if (list != nullptr) {
        delete TO_PARAM_LIST(list);
    }
}

// -------------------------- CefRequest 内存管理 --------------------------
CEFHOST_C_API CefHost_CefRequest CefHost_CefRequest_Create() {
    try {
        return new cefhost::CefRequest();
    }
    catch (...) {
        return nullptr;
    }
}

CEFHOST_C_API void CefHost_CefRequest_Destroy(CefHost_CefRequest req) {
    if (req != nullptr) {
        delete TO_REQUEST(req);
    }
}

// -------------------------- CefResponse 内存管理 --------------------------
CEFHOST_C_API CefHost_CefResponse CefHost_CefResponse_Create() {
    try {
        return new cefhost::CefResponse();
    }
    catch (...) {
        return nullptr;
    }
}

CEFHOST_C_API void CefHost_CefResponse_Destroy(CefHost_CefResponse resp) {
    if (resp != nullptr) {
        delete TO_RESPONSE(resp);
    }
}

// -------------------------- Param 设置值接口 --------------------------
CEFHOST_C_API void CefHost_Param_SetInt32(CefHost_Param param, int32_t value) {
    if (param != nullptr) {
        TO_PARAM(param)->set_intval(value);
    }
}

CEFHOST_C_API void CefHost_Param_SetUInt32(CefHost_Param param, uint32_t value) {
    if (param != nullptr) {
        TO_PARAM(param)->set_uintval(value);
    }
}

CEFHOST_C_API void CefHost_Param_SetInt64(CefHost_Param param, int64_t value) {
    if (param != nullptr) {
        TO_PARAM(param)->set_int64val(value);
    }
}

CEFHOST_C_API void CefHost_Param_SetUInt64(CefHost_Param param, uint64_t value) {
    if (param != nullptr) {
        TO_PARAM(param)->set_uint64val(value);
    }
}

CEFHOST_C_API void CefHost_Param_SetString(CefHost_Param param, const char* value) {
    if (param != nullptr && value != nullptr) {
        TO_PARAM(param)->set_strval(value);
    }
}

CEFHOST_C_API void CefHost_Param_SetBool(CefHost_Param param, bool value) {
    if (param != nullptr) {
        TO_PARAM(param)->set_boolval(value);
    }
}

CEFHOST_C_API void CefHost_Param_SetDouble(CefHost_Param param, double value) {
    if (param != nullptr) {
        TO_PARAM(param)->set_doubleval(value);
    }
}

CEFHOST_C_API void CefHost_Param_SetFloat(CefHost_Param param, float value) {
    if (param != nullptr) {
        TO_PARAM(param)->set_floatval(value);
    }
}

CEFHOST_C_API void CefHost_Param_SetBytes(CefHost_Param param, const uint8_t* bytes, size_t len) {
    if (param != nullptr && bytes != nullptr && len > 0) {
        TO_PARAM(param)->set_binval(std::string(reinterpret_cast<const char*>(bytes), len));
    }
}

CEFHOST_C_API void CefHost_Param_SetArray(CefHost_Param param, CefHost_ParamList list) {
    if (param != nullptr && list != nullptr) {
        // 转移list的所有权到param（Protobuf内部会管理内存）
        cefhost::ParamList* cpp_list = TO_PARAM_LIST(list);
        TO_PARAM(param)->set_allocated_arrayval(cpp_list);
    }
}

// -------------------------- Param 获取值接口 --------------------------
CEFHOST_C_API CefHost_ParamType CefHost_Param_GetType(const CefHost_Param param) {
    if (param == nullptr) return CEFHOST_PARAM_TYPE_NONE;

    switch (TO_CONST_PARAM(param)->value_case()) {
    case cefhost::Param::kIntVal:    return CEFHOST_PARAM_TYPE_INT32;
    case cefhost::Param::kUintVal:   return CEFHOST_PARAM_TYPE_UINT32;
    case cefhost::Param::kInt64Val:  return CEFHOST_PARAM_TYPE_INT64;
    case cefhost::Param::kUint64Val: return CEFHOST_PARAM_TYPE_UINT64;
    case cefhost::Param::kStrVal:    return CEFHOST_PARAM_TYPE_STRING;
    case cefhost::Param::kBoolVal:   return CEFHOST_PARAM_TYPE_BOOL;
    case cefhost::Param::kDoubleVal: return CEFHOST_PARAM_TYPE_DOUBLE;
    case cefhost::Param::kFloatVal:  return CEFHOST_PARAM_TYPE_FLOAT;
    case cefhost::Param::kBinVal:    return CEFHOST_PARAM_TYPE_BYTES;
    case cefhost::Param::kArrayVal:  return CEFHOST_PARAM_TYPE_ARRAY;
    default: return CEFHOST_PARAM_TYPE_NONE;
    }
}

CEFHOST_C_API int32_t CefHost_Param_GetInt32(const CefHost_Param param) {
    return (param != nullptr) ? TO_CONST_PARAM(param)->intval() : 0;
}

CEFHOST_C_API uint32_t CefHost_Param_GetUInt32(const CefHost_Param param) {
    return (param != nullptr) ? TO_CONST_PARAM(param)->uintval() : 0;
}

CEFHOST_C_API int64_t CefHost_Param_GetInt64(const CefHost_Param param) {
    return (param != nullptr) ? TO_CONST_PARAM(param)->int64val() : 0;
}

CEFHOST_C_API uint64_t CefHost_Param_GetUInt64(const CefHost_Param param) {
    return (param != nullptr) ? TO_CONST_PARAM(param)->uint64val() : 0;
}

CEFHOST_C_API const char* CefHost_Param_GetString(const CefHost_Param param) {
    if (param == nullptr || !TO_CONST_PARAM(param)->has_strval()) {
        return ""; // 返回空字符串而非nullptr，避免调用方崩溃
    }
    return TO_CONST_PARAM(param)->strval().c_str();
}

CEFHOST_C_API bool CefHost_Param_GetBool(const CefHost_Param param) {
    return (param != nullptr) ? TO_CONST_PARAM(param)->boolval() : false;
}

CEFHOST_C_API double CefHost_Param_GetDouble(const CefHost_Param param) {
    return (param != nullptr) ? TO_CONST_PARAM(param)->doubleval() : 0.0;
}

CEFHOST_C_API float CefHost_Param_GetFloat(const CefHost_Param param) {
    return (param != nullptr) ? TO_CONST_PARAM(param)->floatval() : 0.0f;
}

CEFHOST_C_API size_t CefHost_Param_GetBytes(const CefHost_Param param, const uint8_t** out_bytes) {
    if (param == nullptr || out_bytes == nullptr || !TO_CONST_PARAM(param)->has_binval()) {
        *out_bytes = nullptr;
        return 0;
    }
    const std::string& bytes = TO_CONST_PARAM(param)->binval();
    *out_bytes = reinterpret_cast<const uint8_t*>(bytes.data());
    return bytes.size();
}

CEFHOST_C_API CefHost_ParamList CefHost_Param_GetArray(const CefHost_Param param) {
    if (param == nullptr || !TO_CONST_PARAM(param)->has_arrayval()) {
        return nullptr;
    }
    // 复制一份ParamList返回（避免内部指针被释放后失效）
    cefhost::ParamList* new_list = new cefhost::ParamList();
    new_list->CopyFrom(TO_CONST_PARAM(param)->arrayval());
    return new_list;
}

// -------------------------- ParamList 操作接口 --------------------------
CEFHOST_C_API void CefHost_ParamList_AddParam(CefHost_ParamList list, CefHost_Param param) {
    if (list == nullptr || param == nullptr) return;
    // 将param的内容复制到list中（param所有权仍归调用方，需手动释放）
    cefhost::Param* cpp_param = TO_PARAM(param);
    TO_PARAM_LIST(list)->add_items()->CopyFrom(*cpp_param);
}

CEFHOST_C_API size_t CefHost_ParamList_GetCount(const CefHost_ParamList list) {
    return (list != nullptr) ? TO_CONST_PARAM_LIST(list)->items_size() : 0;
}

CEFHOST_C_API CefHost_Param CefHost_ParamList_GetParam(const CefHost_ParamList list, size_t index) {
    if (list == nullptr || index >= TO_CONST_PARAM_LIST(list)->items_size()) {
        return nullptr;
    }
    // 复制指定索引的Param返回（避免内部指针失效）
    cefhost::Param* new_param = new cefhost::Param();
    new_param->CopyFrom(TO_CONST_PARAM_LIST(list)->items(index));
    return new_param;
}

// -------------------------- CefRequest 操作接口 --------------------------
CEFHOST_C_API void CefHost_CefRequest_AddParamToList(CefHost_CefRequest req, CefHost_Param param) {
    if (req == nullptr || param == nullptr) return;
    // 复制param到request的paramList中
    cefhost::Param* cpp_param = TO_PARAM(param);
    TO_REQUEST(req)->add_paramlist()->CopyFrom(*cpp_param);
}

CEFHOST_C_API void CefHost_CefRequest_AddParamToMap(CefHost_CefRequest req, const char* key, CefHost_Param param) {
    if (req == nullptr || param == nullptr || key == nullptr) return;
    // 复制param到request的paramMap中
    cefhost::Param* cpp_param = TO_PARAM(param);
    //(*TO_REQUEST(req)->mutable_parammap())[key] = *cpp_param; //等价与CopyFrom
    (*TO_REQUEST(req)->mutable_parammap())[key].CopyFrom(*cpp_param);
}

// -------------------------- CefResponse 操作接口 --------------------------
CEFHOST_C_API void CefHost_CefResponse_AddParamToList(CefHost_CefResponse resp, CefHost_Param param) {
    if (resp == nullptr || param == nullptr) return;
    // 复制param到response的paramList中
    cefhost::Param* cpp_param = TO_PARAM(param);
    TO_RESPONSE(resp)->add_paramlist()->CopyFrom(*cpp_param);
}

CEFHOST_C_API void CefHost_CefResponse_AddParamToMap(CefHost_CefResponse resp, const char* key, CefHost_Param param) {
    if (resp == nullptr || param == nullptr || key == nullptr) return;
    // 复制param到response的paramMap中
    cefhost::Param* cpp_param = TO_PARAM(param);
   // (*TO_RESPONSE(resp)->mutable_parammap())[key] = *cpp_param; //等价与CopyFrom
    (*TO_RESPONSE(resp)->mutable_parammap())[key].CopyFrom(*cpp_param);
}

// -------------------------- 序列化/反序列化接口 --------------------------
CEFHOST_C_API size_t CefHost_CefRequest_Serialize(const CefHost_CefRequest req, uint8_t** out_data) {
    if (req == nullptr || out_data == nullptr) return 0;
    *out_data = nullptr;

    const cefhost::CefRequest* cpp_req = TO_CONST_REQUEST(req);
    std::string serialized = cpp_req->SerializeAsString();
    if (serialized.empty()) return 0;

    // 分配内存并复制数据（调用方需用CefHost_FreeSerializedData释放）
    uint8_t* data = new uint8_t[serialized.size()];
    std::memcpy(data, serialized.data(), serialized.size());
    *out_data = data;
    return serialized.size();
}

CEFHOST_C_API size_t CefHost_CefResponse_Serialize(const CefHost_CefResponse resp, uint8_t** out_data) {
    if (resp == nullptr || out_data == nullptr) return 0;
    *out_data = nullptr;

    const cefhost::CefResponse* cpp_resp = TO_CONST_RESPONSE(resp);
    std::string serialized = cpp_resp->SerializeAsString();
    if (serialized.empty()) return 0;

    // 分配内存并复制数据（调用方需用CefHost_FreeSerializedData释放）
    uint8_t* data = new uint8_t[serialized.size()];
    std::memcpy(data, serialized.data(), serialized.size());
    *out_data = data;
    return serialized.size();
}

CEFHOST_C_API CefHost_CefRequest CefHost_CefRequest_Deserialize(const uint8_t* data, size_t len) {
    if (data == nullptr || len == 0) return nullptr;

    cefhost::CefRequest* cpp_req = new cefhost::CefRequest();
    if (!cpp_req->ParseFromArray(data, len)) {
        delete cpp_req; // 解析失败释放内存
        return nullptr;
    }
    return cpp_req;
}

CEFHOST_C_API CefHost_CefResponse CefHost_CefResponse_Deserialize(const uint8_t* data, size_t len) {
    if (data == nullptr || len == 0) return nullptr;

    cefhost::CefResponse* cpp_resp = new cefhost::CefResponse();
    if (!cpp_resp->ParseFromArray(data, len)) {
        delete cpp_resp; // 解析失败释放内存
        return nullptr;
    }
    return cpp_resp;
}

CEFHOST_C_API void CefHost_FreeSerializedData(uint8_t* data) {
    if (data != nullptr) {
        delete[] data;
    }
}
