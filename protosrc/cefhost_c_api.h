#ifndef CEFHOST_C_API_H
#define CEFHOST_C_API_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// -------------------------- 1.  opaque ���Ͷ��壨�����ڲ�ʵ�֣�--------------------------
// ���� Protobuf ���ɵ� C++ �࣬���÷���ͨ��ָ�����
typedef void* CefHost_Param;          // ��Ӧ Protobuf �� Param ��
typedef void* CefHost_ParamList;      // ��Ӧ Protobuf �� ParamList ��
typedef void* CefHost_CefRequest;     // ��Ӧ Protobuf �� CefRequest ��
typedef void* CefHost_CefResponse;    // ��Ӧ Protobuf �� CefResponse ��

// ��������ö�٣���Ӧ Protobuf Param �� oneof ���ͣ�
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
    CEFHOST_PARAM_TYPE_ARRAY  // ��Ӧ arrayVal��ParamList��
} CefHost_ParamType;

// -------------------------- 2. �ڴ����ӿڣ�����/�ͷţ�--------------------------
// �������ţ�Windows ���� __declspec(dllexport)��Linux �¿�ʡ�ԣ����� -fvisibility=default��
#ifdef _WIN32
#define CEFHOST_C_API __declspec(dllexport)
#else
#define CEFHOST_C_API
#endif

// Param �ڴ����
CEFHOST_C_API CefHost_Param CefHost_Param_Create();          // ���� Param ����
CEFHOST_C_API void CefHost_Param_Destroy(CefHost_Param param); // �ͷ� Param ����

// ParamList �ڴ����
CEFHOST_C_API CefHost_ParamList CefHost_ParamList_Create();  // ���� ParamList ����
CEFHOST_C_API void CefHost_ParamList_Destroy(CefHost_ParamList list); // �ͷ� ParamList ����

// CefRequest/CefResponse �ڴ����
CEFHOST_C_API CefHost_CefRequest CefHost_CefRequest_Create(); // ���� CefRequest ����
CEFHOST_C_API void CefHost_CefRequest_Destroy(CefHost_CefRequest req); // �ͷ� CefRequest

CEFHOST_C_API CefHost_CefResponse CefHost_CefResponse_Create(); // ���� CefResponse ����
CEFHOST_C_API void CefHost_CefResponse_Destroy(CefHost_CefResponse resp); // �ͷ� CefResponse

// -------------------------- 3. Param �����ӿڣ�����/��ȡֵ��--------------------------
// ���� Param ��ֵ����������ѡ���Ӧ������
CEFHOST_C_API void CefHost_Param_SetInt32(CefHost_Param param, int32_t value);
CEFHOST_C_API void CefHost_Param_SetUInt32(CefHost_Param param, uint32_t value);
CEFHOST_C_API void CefHost_Param_SetInt64(CefHost_Param param, int64_t value);
CEFHOST_C_API void CefHost_Param_SetUInt64(CefHost_Param param, uint64_t value);
CEFHOST_C_API void CefHost_Param_SetString(CefHost_Param param, const char* value); // value ���� UTF-8 �ַ���
CEFHOST_C_API void CefHost_Param_SetBool(CefHost_Param param, bool value);
CEFHOST_C_API void CefHost_Param_SetDouble(CefHost_Param param, double value);
CEFHOST_C_API void CefHost_Param_SetFloat(CefHost_Param param, float value);
CEFHOST_C_API void CefHost_Param_SetBytes(CefHost_Param param, const uint8_t* bytes, size_t len); // ����������
CEFHOST_C_API void CefHost_Param_SetArray(CefHost_Param param, CefHost_ParamList list); // �������飨ParamList��

// ��ȡ Param ��ֵ����ͨ�� CefHost_Param_GetType ��ȡ���ͣ��ٵ��ö�Ӧ������
CEFHOST_C_API CefHost_ParamType CefHost_Param_GetType(const CefHost_Param param);
CEFHOST_C_API int32_t CefHost_Param_GetInt32(const CefHost_Param param);
CEFHOST_C_API uint32_t CefHost_Param_GetUInt32(const CefHost_Param param);
CEFHOST_C_API int64_t CefHost_Param_GetInt64(const CefHost_Param param);
CEFHOST_C_API uint64_t CefHost_Param_GetUInt64(const CefHost_Param param);
CEFHOST_C_API const char* CefHost_Param_GetString(const CefHost_Param param); // �����ڲ�ָ�룬������÷��ͷ�
CEFHOST_C_API bool CefHost_Param_GetBool(const CefHost_Param param);
CEFHOST_C_API double CefHost_Param_GetDouble(const CefHost_Param param);
CEFHOST_C_API float CefHost_Param_GetFloat(const CefHost_Param param);
// ��ȡ���������ݣ��������ݳ��ȣ�����ͨ�� out_bytes ������ڲ�ָ�룬�����ͷţ�
CEFHOST_C_API size_t CefHost_Param_GetBytes(const CefHost_Param param, const uint8_t** out_bytes);
CEFHOST_C_API CefHost_ParamList CefHost_Param_GetArray(const CefHost_Param param); // ���ص� array ����÷��ֶ��ͷ�

// -------------------------- 4. ParamList �����ӿڣ����������--------------------------
CEFHOST_C_API void CefHost_ParamList_AddParam(CefHost_ParamList list, CefHost_Param param); // ��� Param �����飨param ����Ȩת�ƣ�������÷��ͷţ�
CEFHOST_C_API size_t CefHost_ParamList_GetCount(const CefHost_ParamList list); // ��ȡ����Ԫ�ظ���
CEFHOST_C_API CefHost_Param CefHost_ParamList_GetParam(const CefHost_ParamList list, size_t index); // ��ȡָ�������� Param�����ص� param ����÷��ͷţ�

// -------------------------- 5. CefRequest/CefResponse �����ӿ�--------------------------
// CefRequest����Ӳ����� paramList / paramMap
CEFHOST_C_API void CefHost_CefRequest_AddParamToList(CefHost_CefRequest req, CefHost_Param param); // param ����Ȩת��
CEFHOST_C_API void CefHost_CefRequest_AddParamToMap(CefHost_CefRequest req, const char* key, CefHost_Param param); // key �� UTF-8 �ַ�����param ����Ȩת��

// CefResponse����Ӳ����� paramList / paramMap���� CefRequest ���ƣ�
CEFHOST_C_API void CefHost_CefResponse_AddParamToList(CefHost_CefResponse resp, CefHost_Param param);
CEFHOST_C_API void CefHost_CefResponse_AddParamToMap(CefHost_CefResponse resp, const char* key, CefHost_Param param);

// -------------------------- 6. ���л�/�����л��ӿڣ�Protobuf ���Ĺ��ܷ�װ��--------------------------
// ���л����� CefRequest/CefResponse תΪ���������ݣ��������ݳ��ȣ�out_data ����÷��� CefHost_FreeSerializedData �ͷţ�
CEFHOST_C_API size_t CefHost_CefRequest_Serialize(const CefHost_CefRequest req, uint8_t** out_data);
CEFHOST_C_API size_t CefHost_CefResponse_Serialize(const CefHost_CefResponse resp, uint8_t** out_data);

// �����л���������������תΪ CefRequest/CefResponse��ʧ�ܷ��� NULL��
CEFHOST_C_API CefHost_CefRequest CefHost_CefRequest_Deserialize(const uint8_t* data, size_t len);
CEFHOST_C_API CefHost_CefResponse CefHost_CefResponse_Deserialize(const uint8_t* data, size_t len);

// �ͷ����л����ɵĶ���������
CEFHOST_C_API void CefHost_FreeSerializedData(uint8_t* data);

#endif // CEFHOST_C_API_H