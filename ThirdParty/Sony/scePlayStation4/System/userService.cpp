#include "userService.h"
#include "../Audio/soundSystem.h"
#include "../Input/gamePad.h"
#include "../Input/keyboard.h"
#include "../Input/mouse.h"

#include <assert.h>
#include <memory>
#include <user_service.h>
#include <sceerror.h>

using namespace System;

namespace {
	const int PLAYER_MAX = 4;
	UserServiceUserId users[PLAYER_MAX];

	user_event_callback loginCallback = NULL;
	user_event_callback logoutCallback = NULL;

	int findOpenSlot()
	{
		for (auto i = 0; i < PLAYER_MAX; i++)
		{
			if (users[i] == SCE_USER_SERVICE_USER_ID_INVALID)
				return i;
		}

		return SCE_USER_SERVICE_USER_ID_INVALID;
	}

	void OnLogin(UserServiceUserId userId)
	{
		auto playerIndex = findOpenSlot();
		if (playerIndex >= 0)
		{
			users[playerIndex] = userId;

			if (loginCallback != NULL)
				loginCallback(userId, playerIndex);

			Input::GamePad::Enable(userId, playerIndex);
			Input::Keyboard::Enable(userId, playerIndex);
			Input::Mouse::Enable(userId, playerIndex);
			Audio::SoundSystem::GetInstance()->OpenControllerPort(playerIndex, userId);
		}
		else
		{
			printf("ERROR: Couldn't assign user %d to player slot!\n", userId);
		}
	}

	void OnLogout(UserServiceUserId userId)
	{
		for (auto playerIndex = 0; playerIndex < PLAYER_MAX; playerIndex++)
		{
			if (users[playerIndex] != userId)
				continue;

			if (logoutCallback != NULL)
				logoutCallback(userId, playerIndex);

			Input::GamePad::Disable(playerIndex);
			Input::Keyboard::Disable(playerIndex);
			Input::Mouse::Disable(playerIndex);
			Audio::SoundSystem::GetInstance()->CloseControllerPort(playerIndex);

			users[playerIndex] = SCE_USER_SERVICE_USER_ID_INVALID;
		}
	}
}

void UserService::Initialize()
{
	for (auto i=0; i < PLAYER_MAX; i++)
		users[i] = SCE_USER_SERVICE_USER_ID_INVALID;

	auto ret = sceUserServiceInitialize(NULL);
	assert(ret == SCE_OK);
	if (ret != SCE_OK)
	{
		printf("ERROR: Couldn't initialize User Service: 0x%08X\n", ret);
		return;
	}

	Input::GamePad::Initialize();
	Input::Keyboard::Initialize();
	Input::Mouse::Initialize();

	// Perform an initial update for already logged in users
	Update(0.0f);
}

void UserService::Terminate()
{
	Input::GamePad::Terminate();
	Input::Keyboard::Terminate();
	Input::Mouse::Terminate();

	auto ret = sceUserServiceTerminate();
	assert(ret == SCE_OK);
	if (ret != SCE_OK)
	{
		printf("ERROR: Couldn't terminate User Service: 0x%08X\n", ret);
		return;
	}
}

void UserService::Update(float elapsedSeconds)
{
	SceUserServiceEvent event;
	for (;;)
	{
		auto ret = sceUserServiceGetEvent(&event);
		if (ret == SCE_USER_SERVICE_ERROR_NO_EVENT)
			break;

		if (ret != SCE_OK)
		{
			printf("ERROR: Couldn't get event from User Service: 0x%08X\n", ret);
			return;
		}

		switch (event.eventType)
		{
		case SCE_USER_SERVICE_EVENT_TYPE_LOGIN:
			OnLogin(event.userId);
			break;

		case SCE_USER_SERVICE_EVENT_TYPE_LOGOUT:
			OnLogout(event.userId);
			break;
		}
	}

	Input::GamePad::Update();
	Input::Keyboard::Update();
	Input::Mouse::Update();
}

void UserService::SetLoginCallback(user_event_callback callback)
{
	loginCallback = callback;
}

void UserService::SetLogoutCallback(user_event_callback callback)
{
	logoutCallback = callback;
}

int UserService::GetPlayerIndexByUserId(SceUserServiceUserId userId)
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		if (users[i] == userId)
			return i;
	}

	return -1;
}

UserServiceUserId UserService::GetUserByPlayerIndex(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);
	if (playerIndex < 0 || playerIndex >= PLAYER_MAX)
	{
		printf("ERROR: Invalid player index %d in %s\n", playerIndex, __FUNCTION__);
		return SCE_USER_SERVICE_USER_ID_INVALID;
	}

	return users[playerIndex];
}

UserServiceUserId UserService::GetInitialUser()
{
	UserServiceUserId result;
	auto error = sceUserServiceGetInitialUser(&result);
	if(error < SCE_OK)
		return SCE_USER_SERVICE_USER_ID_INVALID;

	return result;
}