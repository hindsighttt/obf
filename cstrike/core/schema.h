#pragma once


#include "../common.h"

// used: fnv-1a hash
#include "../utilities/fnv1a.h"

#define CALL_VIRTUAL(retType, idx, ...) \
    vmt::CallVirtual<retType>(idx, __VA_ARGS__)

#define SCHEMA_ADD_OFFSET(TYPE, NAME, OFFSET)                                                                 \
	[[nodiscard]] CS_INLINE std::add_lvalue_reference_t<TYPE> NAME()                                          \
	{                                                                                                         \
		static const std::uint32_t uOffset = OFFSET;                                                          \
		return *reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + (uOffset)); \
	}

#define SCHEMA_ADD_POFFSET(TYPE, NAME, OFFSET)                                                               \
	[[nodiscard]] CS_INLINE std::add_pointer_t<TYPE> NAME()                                                  \
	{                                                                                                        \
		const static std::uint32_t uOffset = OFFSET;                                                         \
		return reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + (uOffset)); \
	}

#define SCHEMA_ADD_FIELD_OFFSET(TYPE, NAME, FIELD, ADDITIONAL) SCHEMA_ADD_OFFSET(TYPE, NAME, SCHEMA::GetOffset(FNV1A::HashConst(FIELD)) + ADDITIONAL)

#define SCHEMA_ADD_FIELD(TYPE, NAME, FIELD) SCHEMA_ADD_FIELD_OFFSET(TYPE, NAME, FIELD, 0U)

#define SCHEMA_ADD_PFIELD_OFFSET(TYPE, NAME, FIELD, ADDITIONAL) SCHEMA_ADD_POFFSET(TYPE, NAME, SCHEMA::GetOffset(FNV1A::HashConst(FIELD)) + ADDITIONAL)

#define SCHEMA_ADD_PFIELD(TYPE, NAME, FIELD) SCHEMA_ADD_PFIELD_OFFSET(TYPE, NAME, FIELD, 0U)

// @todo: dump enums?
namespace SCHEMA
{
	// store the offset of the field in the class
	// dump stored data to file
	bool Setup(const wchar_t* wszFileName, const char* szModuleName);

	/* @section: get */
	// get offset of the field in the class
	// @note: only client.dll class & fields
	[[nodiscard]] std::uint32_t GetOffset(const FNV1A_t uHashedFieldName);

	// get foregin offset from other .dll
	[[nodiscard]] std::uint32_t GetForeignOffset(const char* szModulenName, const FNV1A_t uHashedClassName, const FNV1A_t uHashedFieldName);
}

namespace vmt {
    template <typename T = void*>
    inline T GetVMethod(uint32_t uIndex, void* pClass) {
        if (!pClass) {
            //LOG("Tried getting virtual function from a null class.\n");
            return T{};
        }

        void** pVTable = *static_cast<void***>(pClass);
        if (!pVTable) {
            //LOG("Tried getting virtual function from a null vtable.\n");
            return T{};
        }

        return reinterpret_cast<T>(pVTable[uIndex]);
    }

    template <typename T, typename... Args>
    inline T CallVirtual(uint32_t uIndex, void* pClass, Args... args) {
        auto pFunc = GetVMethod<T(__thiscall*)(void*, Args...)>(uIndex, pClass);
        if (!pFunc) {
            //LOG("Tried calling a null virtual function.\n");
            return T{};
        }

        return pFunc(pClass, args...);
    }

    template <typename T, typename... Args>
    inline T call_virtual(void* class_, unsigned int index, Args... args) {
        auto func = GetVMethod<T(__thiscall*)(void*, Args...)>(index, class_);

        return func(class_, args...);
    }
}  // namespace vmt