#pragma once
#include "predecls.h"

#include <user_service.h>

namespace System {

enum class UserServiceUserColor
{
	Blue = SCE_USER_SERVICE_USER_COLOR_BLUE,
	Red = SCE_USER_SERVICE_USER_COLOR_RED,
	Green = SCE_USER_SERVICE_USER_COLOR_GREEN,
	Pink = SCE_USER_SERVICE_USER_COLOR_PINK,
};

class CS_API UserService
{
	UserService() {}

public:

	// TODO:  This would all be much better if they could be 
	// private, but still exported by CppSharp... like a CS_EXPORT.

	typedef void (*_UserCallback)(UserServiceUserId userId, int playerIndex);

	static void _SetLoginCallback(_UserCallback callback);
	static void _SetLogoutCallback(_UserCallback callback);

private:

	static bool _initialized;

	static const int PLAYER_MAX = 4;
	static UserServiceUserId _users[PLAYER_MAX];
	static _UserCallback _loginCallback;
	static _UserCallback _logoutCallback;

	static void OnLogin(UserServiceUserId userId);
	static void OnLogout(UserServiceUserId userId);
	static int FindOpenSlot();

public:
	static void Initialize();
	static void Terminate();
	static void Update(float elapsedSeconds);

	static int GetPlayerIndexByUserId(UserServiceUserId userId);
	static UserServiceUserId GetUserByPlayerIndex(int playerIndex);

	static const char* GetUserName(UserServiceUserId userId);

	static UserServiceUserColor GetUserColor(UserServiceUserId userId);

	static UserServiceUserId GetInitialUser();


};

} // namespace System
