#include "userService.h"
#include "Input/gamePad.h"

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

	// We're supposed to get a login event for everyone from initialization
	// onward, we don't always (could be the timing of calling this method from
	// managed code. So we initialize the existing users from the start here.
	SceUserServiceLoginUserIdList initialUsers;
	ret = sceUserServiceGetLoginUserIdList(&initialUsers);
	assert(ret == SCE_OK);
	for (auto i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++)
		Input::GamePad::Enable(initialUsers.userId[i]);
}

void UserService::Terminate()
{
	Input::GamePad::Terminate();

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

			break;

		case SCE_USER_SERVICE_EVENT_TYPE_LOGOUT:
			if (logoutCallback != NULL)
				logoutCallback(event.userId);

			Input::GamePad::Disable(event.userId);

			break;
		}
	}

	Input::GamePad::Update(elapsedSeconds);
}

void UserService::SetLoginCallback(user_event_callback callback)
{
	loginCallback = callback;
}

void UserService::SetLogoutCallback(user_event_callback callback)
{
	logoutCallback = callback;
}
