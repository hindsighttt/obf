#pragma once
#include <string>
#include <cstdint>
#include "../../math/math.hpp"
#define COMBINE(x, y) x##y
#define COMBINE2(x, y) COMBINE(x, y)

#define PAD_CLASS(sz)                             \
   private:                                       \
    std::uint8_t COMBINE2(pad_, __COUNTER__)[sz]; \
                                                  \
   public:
enum { pipiska = 1 << 0 };
enum ECommandButtons : __int32 {
    IN_ATTACK = (1 << 0),
    IN_JUMP = (1 << 1),
    IN_DUCK = (1 << 2),
    IN_FORWARD = (1 << 3),
    IN_BACK = (1 << 4),
    IN_USE = (1 << 5),
    IN_CANCEL = (1 << 6),
    IN_LEFT = (1 << 7),
    IN_RIGHT = (1 << 8),
    IN_MOVELEFT = (1 << 9),
    IN_MOVERIGHT = (1 << 10),
    IN_SECOND_ATTACK = (1 << 11),
    IN_RUN = (1 << 12),
    IN_RELOAD = (1 << 13),
    IN_LEFT_ALT = (1 << 14),
    IN_RIGHT_ALT = (1 << 15),
    IN_SCORE = (1 << 16),
    IN_SPEED = (1 << 17),
    IN_WALK = (1 << 18),
    IN_ZOOM = (1 << 19),
    IN_FIRST_WEAPON = (1 << 20),
    IN_SECOND_WEAPON = (1 << 21),
    IN_BULLRUSH = (1 << 22),
    IN_FIRST_GRENADE = (1 << 23),
    IN_SECOND_GRENADE = (1 << 24),
    IN_MIDDLE_ATTACK = (1 << 25),
    IN_USE_OR_RELOAD = (1 << 26)
};
class CBasePB {
    void* pVTable;              // 0x0
    std::uint32_t nHasBits;     // 0x8
    std::uint64_t nCachedBits;  // 0xC
};
static_assert(sizeof(CBasePB) == 0x18);
class CCmdQAngle : public CBasePB {
   public:
    QAngle_t angValue;  // 0x18
};

class CCmdVector : public CBasePB {
   public:
    Vector4D vecValue;  // 0x18
};

class CCSGOInterpolationInfo : public CBasePB {
   public:
    float flFraction;  // 0x18
    int nSrcTick;      // 0x1C
    int nDstTick;      // 0x20
};

class CCSGOInputHistoryEntryPB : public CBasePB {
   public:
    CCmdQAngle* pViewCmd;                   // 0x18
    CCmdVector* pShootOriginCmd;            // 0x20
    CCmdVector* pTargetHeadOriginCmd;       // 0x28
    CCmdVector* pTargetAbsOriginCmd;        // 0x30
    CCmdQAngle* pTargetViewCmd;             // 0x38
    CCSGOInterpolationInfo* cl_interp;      // 0x40
    CCSGOInterpolationInfo* sv_interp0;     // 0x48
    CCSGOInterpolationInfo* sv_interp1;     // 0x50
    CCSGOInterpolationInfo* player_interp;  // 0x58
    int nRenderTickCount;                   // 0x60
    float flRenderTickFraction;             // 0x64
    int nPlayerTickCount;                   // 0x68
    float flPlayerTickFraction;             // 0x6C
    int nFrameNumber;                       // 0x70
    int nTargetEntIndex;                    // 0x74
};


template <typename T>
struct RepeatedPtrField_t
{
    struct Rep_t
    {
        int nAllocatedSize;
        T* tElements[(std::numeric_limits<int>::max() - 2 * sizeof(int)) / sizeof(void*)];
    };

    void* pArena;
    int nCurrentSize;
    int nTotalSize;
    Rep_t* pRep;
};

struct CInButtonStatePB : CBasePB {
    std::uint64_t State1;
    std::uint64_t State2;
    std::uint64_t State3;
};
struct CSubtickMoveStep : CBasePB {
    uint64_t nButton;
    bool bPressed;
    float flWhen;
    float flAnalogForwardDelta;
    float flAnalogLeftDelta;
};
struct CInButtonState {
    void* pVTable;
    uint64_t nButtonState1;
    uint64_t nButtonState2;
    uint64_t nButtonState3;
};
class CBaseUserCmdPB : public CBasePB {
public:
    RepeatedPtrField_t<CSubtickMoveStep> subtickMovesField;
    std::string* strMoveCrc;
    CInButtonState* pInButtonState;  // 0x20
    CCmdQAngle* pViewangles;         // 0x40
    std::int32_t nLegacyCommandNumber;
    std::int32_t nClientTick;
    float flForwardMove;
    float flSideMove;
    float flUpMove;
    std::int32_t nImpulse;
    std::int32_t nWeaponSelect;
    std::int32_t nRandomSeed;
    std::int32_t nMousedX;
    std::int32_t nMousedY;
    std::uint32_t nConsumedServerAngleChanges;
    std::int32_t nCmdFlags;
    std::uint32_t nPawnEntityHandle;
};
struct CSubTickCMD {
    CCSGOInputHistoryEntryPB* inputhistory;
    uint8_t _pad[0xe];
    CCmdQAngle* view;
};
class CSubTickContainer {
   public:
    int32_t nTickCount;     // 0x0000
    char pad_0x0004[0x4];   // 0x0004
    uint8_t* pTickPointer;  // 0x0008

    CSubTickCMD* GetTick(std::int32_t index) {
        if (index < this->nTickCount) {
            CSubTickCMD** tick_list =
                reinterpret_cast<CSubTickCMD**>(this->pTickPointer + 0x8);
            return tick_list[index];
        }

        return nullptr;
    }

};  // Size=0x0010

class CCSGOUserCmdPB 
{
   public:
    std::uint32_t nHasBits;
    std::uint64_t nCachedSize;
    RepeatedPtrField_t<CCSGOInputHistoryEntryPB> inputHistoryField;
    CBaseUserCmdPB* pBase;
    bool bLeftHandDesired;
    std::int32_t nAttack3StartHistoryIndex;
    std::int32_t nAttack1StartHistoryIndex;
    std::int32_t nAttack2StartHistoryIndex;
};
struct StartHistoryIndexTick_t {
    int nAttack1;
    int nAttack2;
    int nAttack3;
};

class CUserCmd 
{
public:
    PAD_CLASS(0x18);
    CCSGOUserCmdPB pBase;
    CInButtonState buttonStates;
    PAD_CLASS(0x20)

    CCSGOInputHistoryEntryPB* GetInputHistoryEntry(int nIndex)
    {
        if (nIndex >= pBase.inputHistoryField.pRep->nAllocatedSize || nIndex >= pBase.inputHistoryField.nCurrentSize)
            return nullptr;

        return pBase.inputHistoryField.pRep->tElements[nIndex];
    }

};
