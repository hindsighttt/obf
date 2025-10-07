#include "notify.h"

// used: cheat variables
#include "../core/variables.h"
// used: menu::bMainWindowOpened
#include "../core/menu.h"
// used: [resources] font awesome icons definitions
#include "../../resources/font_awesome_5.h"
// used: easingg library
#include "easing.h"

inline static const float GetTime() noexcept
{
	return static_cast<float>(clock() / 1000.f);
}

inline static bool IsEmptyOrNullptr(const char* szText)
{
	return szText == nullptr || szText[0] == '\0';
}

NOTIFY::NotificationData_t::NotificationData_t(ENotificationType nType, const char* szFormat, ...) :
	nType(nType), flCreateionTime(GetTime()), animHandler(&EASING::InQuad, &EASING::OutQuad)
{
	va_list args;
	va_start(args, szFormat);
	stbsp_vsnprintf(this->szBuffer, sizeof(szBuffer), szFormat, args);
	va_end(args);
}

const Color_t& NOTIFY::NotificationData_t::GetTypeColor() const
{
	switch (nType)
	{
	case N_TYPE_INFO:
		// cyan
		return Color_t(0, 255, 255);
	case N_TYPE_SUCCESS:
		// green
		return Color_t(0, 255, 0);
	case N_TYPE_WARNING:
		// yellow
		return Color_t(255, 255, 0);
	case N_TYPE_ERROR:
		// red
		return Color_t(255, 0, 0);
	default:
		// white
		return Color_t(255, 255, 255);
	}
}

const char* NOTIFY::NotificationData_t::Data() const
{
	return this->szBuffer;
}

const char* NOTIFY::NotificationData_t::GetIcon() const
{
	switch (nType)
	{
	case N_TYPE_INFO:
		return ICON_FA_INFO;
	case N_TYPE_SUCCESS:
		return ICON_FA_CHECK;
	case N_TYPE_WARNING:
		return ICON_FA_EXCLAMATION;
	case N_TYPE_ERROR:
		return ICON_FA_TIMES;
	default:
		return nullptr;
	}
}

const float NOTIFY::NotificationData_t::GetTimeDelta(const float flCurrentTime) const
{
	return flCurrentTime - this->flCreateionTime;
}

void NOTIFY::Push(const NotificationData_t& notification)
{
	vecNotifications.push_back(notification);
}

void NOTIFY::_Remove(size_t nIndex)
{
	vecNotifications.erase(vecNotifications.begin() + nIndex);
}

void NOTIFY::Render()
{
	// Implement new notifications
}