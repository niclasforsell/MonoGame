#pragma once
#include "predecls.h"

typedef int32_t SceUserServiceUserId;
typedef void (*user_event_callback)(SceUserServiceUserId userId, int playerIndex);

class CS_API UserService
{
	UserService() {}

public:
	static void Initialize();
	static void Terminate();
	static void Update(float elapsedSeconds);

	static void SetLoginCallback(user_event_callback callback);
	static void SetLogoutCallback(user_event_callback callback);

	static int GetPlayerIndexByUserId(SceUserServiceUserId userId);
	static SceUserServiceUserId GetUserByPlayerIndex(int playerIndex);
};
