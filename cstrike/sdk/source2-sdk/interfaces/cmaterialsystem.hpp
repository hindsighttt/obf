#include "../types/chandle.hpp"
#include "../memory_classes/utlbuffer.hpp"
#include "../../virtual.hpp"
#include "../../memory/memory.hpp"
#include "../../../api/hook/hook.hpp"
#include "../../memory/memory.hpp"

struct color
{
public:
	float r, g, b, a;
};

class CMaterial2
{
public:
	virtual const char* GetName( ) = 0;
	virtual const char* GetShareName( ) = 0;
};
enum kv_basic_type : uint8_t
{
	kv_basic_invalid,
	kv_basic_null,
	kv_basic_bool,
	kv_basic_int,
	kv_basic_uint,
	kv_basic_double,
	kv_basic_string,
	kv_basic_binary_blob,
	kv_basic_array,
	kv_basic_table
};

enum kv_field_type_t : uint8_t
{
	kv_field_invalid,
	kv_field_resource,
	kv_field_resource_name,
	kv_field_panorama,
	kv_field_soundevent,
	kv_field_subclass,
	kv_field_unspecified,
	kv_field_null,
	kv_field_binary_blob,
	kv_field_array,
	kv_field_table,
	kv_field_bool8,
	kv_field_char8,
	kv_field_uchar32,
	kv_field_int8,
	kv_field_uint8,
	kv_field_int16,
	kv_field_uint16,
	kv_field_int32,
	kv_field_uint32,
	kv_field_int64,
	kv_field_uint64,
	kv_field_float32,
	kv_field_float64,
	kv_field_string,
	kv_field_pointer,
	kv_field_color32,
	kv_field_vector,
	kv_field_vector2d,
	kv_field_vector4d,
	kv_field_rotation_vector,
	kv_field_quaternion,
	kv_field_qangle,
	kv_field_matrix3x4,
	kv_field_transform,
	kv_field_string_token,
	kv_field_ehandle
};

struct CUtlString
{
	const char* Get( )
	{
		return reinterpret_cast<const char*>(m_Memory.m_pMemory);
	}

	CUtlMemory<uint8_t> m_Memory;
	int m_iActualLength;
};

// idk
struct KV3ID_t
{
	const char* name;
	uint64_t unk0;
	uint64_t unk1;
};
class KeyValues3
{
private:
	char pad01[0x100];
	std::uint64_t uKey;
	void* pValue;
	char pad02[0x8];

public:
	// KeyValues3( );
	//~KeyValues3();

	// void* operator new(size_t iAllocSize);
	// void operator delete(void* pMem);
	static KeyValues3& Get( )
	{
		static KeyValues3 inst;
		return inst;
	}
	KeyValues3* set_type(
		kv_basic_type a1,
		kv_basic_type a2);
	bool load_from_buffer(CUtlBuffer* buffer, const char* name);
	void LoadFromBuffer(const char* szString);
	void del(void* pMem);

	KeyValues3* SetType(uint8_t a1, uint8_t a2);

	bool LoadKV3FromKV3OrKV1(CUtlString* ErrorStr, CUtlBuffer* Text, const char** szStrangeConvertType, const char* a4);

	KeyValues3* FreeAlloc(uint32_t nSize);
};
// class KeyValues3 {
//    public:
//     static KeyValues3& Get() {
//         static KeyValues3 inst;
//         return inst;
//     }
//     uint64_t key;
//     void* value;
//     char padding[0x8];
//     void del(void* pMem);
// };

class CObjectInfo
{
	char _pad_001[0xB0];
	int nId;
};

class CSceneAnimatableObject
{
public:
	char _pad_001[0xB8];
	CHandle hOwner;
};

// the naming is incorrect but i dont care atm
class CMeshData
{
public:
	void SetShaderType(const char* szShaderName)
	{
		// @ida: #STR: shader, spritecard.vfx
		MaterialKeyVar_t shaderVar(0x162C1777, "shader");
		memory::fnSetMaterialShaderType(this, shaderVar, szShaderName, 0x18);
	}

	void SetMaterialFunction(const char* szFunctionName, int nValue)
	{
		MaterialKeyVar_t functionVar(szFunctionName, true);
		memory::fnSetMaterial(this, functionVar, nValue, 0x18);
	}

	char _pad_001[0x18];                             // 0x0
	CSceneAnimatableObject* pSceneAnimatableObject;  // 0x10
	CMaterial2* pMaterial;                           // 0x18
	char _pad_002[0x10];                             // 0x20
	color colValue;                                  // 0x40
	char _pad_003[0x8];                              // 0x44
	CObjectInfo* pObjectInfo;                        // 0x48
};

class IMaterialSystem2
{
public:
	CMaterial2*** FindOrCreateFromResource(CMaterial2*** pOutMaterial,
										   const char* szMaterialName)
	{
		return CALL_VIRTUAL(CMaterial2***, 14, this, pOutMaterial,
							szMaterialName);
	}

	PVOID CreateMaterial(PVOID pOutMaterial, const char* szMaterialName, PVOID material_data)
	{
		return CALL_VIRTUAL(PVOID, 29, this, pOutMaterial, szMaterialName, material_data, 0, 1);
	}
	CMaterial2** CreateMaterial_FromData(CMaterial2*** pOutMaterial,
										 const char* szMaterialName,
										 CMeshData* material_data)
	{
		return CALL_VIRTUAL(CMaterial2**, 29, this, pOutMaterial,
							szMaterialName,
							material_data, 0, 0, 0, 0, 0, 1);
	}
};

#define OFFSET( t, n, o )\
__forceinline t& n( )\
{\
	return *reinterpret_cast< t* >( reinterpret_cast< uintptr_t >( this ) + (o));\
};

#define POFFSET( t, n, o )\
__forceinline t* n( )\
{\
	return reinterpret_cast< t* >( reinterpret_cast< uintptr_t >( this ) + (o));\
};

struct cmyk_color_t
{
	union
	{
		struct
		{
			uint8_t c, m, y, k;
		};

		uint32_t m_cmyk;
	};
};

struct object_info_t
{
	enum e_object_id : uint32_t
	{
		arm = 38,
		player_t = 104,
		player_ct = 113,
	};

	OFFSET(e_object_id, m_object_id, 0xb0);
};

struct CMaterial2_Data_t
{
	unsigned char __pad0000[0x18];
	CSceneAnimatableObject* m_pSceneAnimatableObject;
	CMaterial2* m_material;
	unsigned char __pad0028[0x26];
	//color m_color;
	unsigned char __pad002c[0x04];

	OFFSET(cmyk_color_t, m_color, 0x50);
	POFFSET(object_info_t, m_object_info, 0x40);
};

inline static CHook<void __fastcall(void*, void*, CMaterial2_Data_t*, int,
	void*, void*, void*, void*)>  g_DrawObject;
inline void(__fastcall* fnDrawObject)(void*, void*, CMaterial2_Data_t*, int,
									  void*, void*, void*, void*);
inline bool(__fastcall* fnLoadKV3)(KeyValues3*, void*, CUtlBuffer*, const KV3ID_t*, const char*);
inline KeyValues3* (__fastcall* fnSetType)(KeyValues3*, uint8_t, uint8_t);

inline void* pFnFreeAlloc = nullptr;