#pragma once

class CInputSystem
{
public:
	bool& IsRelativeMouseMode( )
	{
		// Offset in 'CInputSystem::SetRelativeMouseMode'.
		// 'CInputSystem::SetRelativeMouseMode' index is 76.
		return *reinterpret_cast<bool*>((uintptr_t) (this) + 0x4F);
	}

	bool vcall_IsRelativeMouseMode( )
	{
		return reinterpret_cast<bool(__thiscall*)(void*)>(vmt::GetVMethod(76, this));
	};

	void* GetSDLWindow( )
	{
		// Offset in 'CInputSystem::DebugSpew'.
		// xref: "Current coordinate bias %s: %g,%g scale %g,%g\n".
		return *reinterpret_cast<void**>((uintptr_t) (this) + 0x2678);
	}
};
