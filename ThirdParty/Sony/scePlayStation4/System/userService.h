#pragma once
#include "predecls.h"

namespace System {

typedef void (*user_event_callback)(UserServiceUserId userId, int playerIndex);

class CS_API UserService
{
	UserService() {}

	static bool _initialized;

public:
	static void Initialize();
	static void Terminate();
	static void Update(float elapsedSeconds);

	static void SetLoginCallback(user_event_callback callback);
	static void SetLogoutCallback(user_event_callback callback);

	static int GetPlayerIndexByUserId(UserServiceUserId userId);
	static UserServiceUserId GetUserByPlayerIndex(int playerIndex);

	static UserServiceUserId GetInitialUser();
};

} // namespace System
