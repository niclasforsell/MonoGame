#include "userService.h"
#include "Input/gamePad.h"
#include "Input/keyboard.h"
#include "Input/mouse.h"

#include <assert.h>
#include <user_service.h>
#include <sceerror.h>

namespace {
	SceUserServiceLoginUserIdList loggedInUsers;

	user_event_callback loginCallback = NULL;
	user_event_callback logoutCallback = NULL;
}

void UserService::Initialize()
{
	auto ret = sceUserServiceInitialize(NULL);
	assert(ret == SCE_OK);

	Input::GamePad::Initialize();
	Input::Keyboard::Initialize();
	Input::Mouse::Initialize();
}

void UserService::Terminate()
{
	Input::GamePad::Terminate();
	Input::Keyboard::Terminate();
	Input::Mouse::Terminate();

	auto ret = sceUserServiceTerminate();
	assert(ret == SCE_OK);
}

void UserService::Update(float elapsedSeconds)
{
	SceUserServiceEvent event;
	for (;;)
	{
		auto ret = sceUserServiceGetEvent(&event);
		if (ret == SCE_USER_SERVICE_ERROR_NO_EVENT)
			break;

		switch (event.eventType)
		{
		case SCE_USER_SERVICE_EVENT_TYPE_LOGIN:
			if (loginCallback != NULL)
				loginCallback(event.userId);

			Input::GamePad::Enable(event.userId);
			Input::Keyboard::Enable(event.userId);
			Input::Mouse::Enable(event.userId);

			break;

		case SCE_USER_SERVICE_EVENT_TYPE_LOGOUT:
			if (logoutCallback != NULL)
				logoutCallback(event.userId);

			Input::GamePad::Disable(event.userId);
			Input::Keyboard::Disable(event.userId);
			Input::Mouse::Disable(event.userId);

			break;
		}
	}

	Input::GamePad::Update(elapsedSeconds);
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
