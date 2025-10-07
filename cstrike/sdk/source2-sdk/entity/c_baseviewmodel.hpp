#pragma once

#include "c_basemodelentity.hpp"

class C_BaseViewModel : public C_BaseModelEntity {
   public:
    SCHEMA_FIELD(m_hWeapon, "C_BaseViewModel", "m_hWeapon", CHandle);
};

class CAnimGraphNetworkedVariables;
class CAnimationGraphInstance2
{
public:
	char pad_0x0000[0x2E0]; //0x0000
	CAnimGraphNetworkedVariables* pAnimGraphNetworkedVariables; //0x02E0
};

class C_CSGOViewModel2 : public C_BaseViewModel
{
public:
	char pad_0x0000[0xD60]; //0x0000
	CAnimationGraphInstance2* pAnimationGraphInstance; //0xD60
};
