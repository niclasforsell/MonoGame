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

bool UserService::_initialized = false;
UserService::_UserCallback UserService::_loginCallback = NULL;
UserService::_UserCallback UserService::_logoutCallback = NULL;
UserServiceUserId UserService::_users[PLAYER_MAX];


int UserService::FindOpenSlot()
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		if (_users[i] == SCE_USER_SERVICE_USER_ID_INVALID)
			return i;
	}

	return SCE_USER_SERVICE_USER_ID_INVALID;
}

void UserService::OnLogin(UserServiceUserId userId)
{
	auto playerIndex = FindOpenSlot();
	if (playerIndex >= 0)
	{
		_users[playerIndex] = userId;

		if (_loginCallback != NULL)
			_loginCallback(userId, playerIndex);

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

void UserService::OnLogout(UserServiceUserId userId)
{
	for (auto playerIndex = 0; playerIndex < PLAYER_MAX; playerIndex++)
	{
		if (_users[playerIndex] != userId)
			continue;

		if (_logoutCallback != NULL)
			_logoutCallback(userId, playerIndex);

		Input::GamePad::Disable(playerIndex);
		Input::Keyboard::Disable(playerIndex);
		Input::Mouse::Disable(playerIndex);
		Audio::SoundSystem::GetInstance()->CloseControllerPort(playerIndex);

		_users[playerIndex] = SCE_USER_SERVICE_USER_ID_INVALID;
	}
}

void UserService::Initialize()
{
	if (_initialized)
		return;

	auto ret = sceUserServiceInitialize(NULL);
	if (ret != SCE_OK)
	{
		if (ret == SCE_USER_SERVICE_ERROR_ALREADY_INITIALIZED)
			printf("WARNING: UserService was already initialized.\n");
		else
		{
			assert(ret == SCE_OK);
			printf("ERROR: Couldn't initialize UserService: 0x%08X\n", ret);
			return;
		}
	}

	for (auto i=0; i < PLAYER_MAX; i++)
		_users[i] = SCE_USER_SERVICE_USER_ID_INVALID;

	_initialized = true;

	Input::GamePad::Initialize();
	Input::Keyboard::Initialize();
	Input::Mouse::Initialize();

	// Perform an initial update for already logged in users
	Update(0.0f);
}

void UserService::Terminate()
{
	if (!_initialized)
		return;

	Input::GamePad::Terminate();
	Input::Keyboard::Terminate();
	Input::Mouse::Terminate();

	auto ret = sceUserServiceTerminate();
	assert(ret == SCE_OK);
	if (ret != SCE_OK)
		printf("ERROR: Couldn't terminate User Service: 0x%08X\n", ret);

	for (auto i=0; i < PLAYER_MAX; i++)
		_users[i] = SCE_USER_SERVICE_USER_ID_INVALID;

	_initialized = false;
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

void UserService::_SetLoginCallback(_UserCallback callback)
{
	_loginCallback = callback;
}

void UserService::_SetLogoutCallback(_UserCallback callback)
{
	_logoutCallback = callback;
}

int UserService::GetPlayerIndexByUserId(SceUserServiceUserId userId)
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		if (_users[i] == userId)
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

	return _users[playerIndex];
}

UserServiceUserId UserService::GetInitialUser()
{
	UserServiceUserId result;
	auto error = sceUserServiceGetInitialUser(&result);
	if(error < SCE_OK)
		return SCE_USER_SERVICE_USER_ID_INVALID;

	return result;
}

const char* UserService::GetUserName(UserServiceUserId userId)
{
	// TODO: This is not thread safe!
	static char temp[SCE_USER_SERVICE_MAX_USER_NAME_LENGTH + 1];
	auto result = sceUserServiceGetUserName(userId, temp, sizeof(temp));
	if (result != SCE_OK)
		return NULL;

	return temp;
}

UserServiceUserColor UserService::GetUserColor(UserServiceUserId userId)
{
	auto color = SCE_USER_SERVICE_USER_COLOR_BLUE;
	sceUserServiceGetUserColor(userId, &color);
	return (UserServiceUserColor)color;
}

