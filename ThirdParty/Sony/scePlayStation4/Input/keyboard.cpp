#include "keyboard.h"
#include "keyboardState.h"
#include "../allocator.h"
#include "vkeys.h"

#include <cstdio>
#include <assert.h>
#include <sceerror.h>
#include <libsysmodule.h>
#include <libime.h>
#include <system_service.h>

using namespace Input;

namespace {
	const int PLAYER_MAX = 4;
	const int SYSTEM_UI_SLEEP_MICROSEC = 5000;

	bool isActive;
	KeyboardState* states[PLAYER_MAX];
	SceUserServiceUserId users[PLAYER_MAX];

	int findOpenSlot()
	{
		for (auto i = 0; i < PLAYER_MAX; i++)
		{
			if (users[i] == -1)
				return i;
		}

		return -1;
	}

	KeyboardState* findStateByUser(SceUserServiceUserId userId)
	{
		for (auto i = 0; i < PLAYER_MAX; i++)
		{
			if (users[i] == userId)
				return states[i];
		}

		return nullptr;
	}

	void HandleKeyboard(void* arg, const SceImeEvent* e)
	{
		auto userId = e->param.keycode.userId;

		auto usbKeyCode = (uint8_t)e->param.keycode.keycode;
		auto virtKeyCode = toVKcode[usbKeyCode];
		auto mask = (uint32_t)1 << (((uint32_t)virtKeyCode) & 0x1f);
		auto keys = (uint32_t)virtKeyCode >> 5;

		switch(e->id)
		{
		case SCE_IME_KEYBOARD_EVENT_ABORT:
			// Per the sceImeKeyboardOpen docs, an abort event can occur immediately
			// after calling open, in which case we should close and re-open.
			Keyboard::Restart();
			break;

		case SCE_IME_KEYBOARD_EVENT_KEYCODE_DOWN:
			{
				auto state = findStateByUser(userId);
				if (state == nullptr)
				{
					printf("WARNING: User %d not found for keydown event.\n", userId);
					return;
				}

				switch(keys)
				{
				case 0: state->keys0 |= mask; break;
				case 1: state->keys1 |= mask; break;
				case 2: state->keys2 |= mask; break;
				case 3: state->keys3 |= mask; break;
				case 4: state->keys4 |= mask; break;
				case 5: state->keys5 |= mask; break;
				case 6: state->keys6 |= mask; break;
				case 7: state->keys7 |= mask; break;
				}
			}
			break;

		case SCE_IME_KEYBOARD_EVENT_KEYCODE_UP:
			{
				auto state = findStateByUser(userId);
				if (state == nullptr)
				{
					printf("WARNING: User %d not found for keyup event.\n", userId);
					return;
				}

				switch(keys)
				{
				case 0: state->keys0 &= ~mask; break;
				case 1: state->keys1 &= ~mask; break;
				case 2: state->keys2 &= ~mask; break;
				case 3: state->keys3 &= ~mask; break;
				case 4: state->keys4 &= ~mask; break;
				case 5: state->keys5 &= ~mask; break;
				case 6: state->keys6 &= ~mask; break;
				case 7: state->keys7 &= ~mask; break;
				}
			}
			break;
		}
	}
}

void Keyboard::Initialize()
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_LIBIME) != SCE_OK)
	{
		for (auto i = 0; i < PLAYER_MAX; i++)
		{
			states[i] = (KeyboardState*)Allocator::Get()->allocate(sizeof(KeyboardState));
			memset(states[i], 0, sizeof(KeyboardState));
			users[i] = -1;
		}

		auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_LIBIME);
		if (ret < 0)
		{
			printf("ERROR: Couldn't initialize keyboard input library: 0x%08X\n", ret);
			return;
		}
	}

	SceImeKeyboardParam param;
	memset(&param, 0, sizeof(SceImeKeyboardParam));
	param.handler = HandleKeyboard;
	param.option = SCE_IME_KEYBOARD_OPTION_DEFAULT;
	memset(param.reserved1, 0, sizeof(param.reserved1));
	memset(param.reserved2, 0, sizeof(param.reserved2));

	// Allow for tracking multiple keyboards in parallel by opening the
	// special everyone user and tracking the user id from the update handler.
	auto ret = sceImeKeyboardOpen(SCE_USER_SERVICE_USER_ID_EVERYONE, &param);

	// If we're waiting on the system UI, the first call may return
	// a not active error. The docs suggest periodically retrying in
	// that case.
	while (ret == SCE_IME_ERROR_NOT_ACTIVE)
	{
		sceKernelUsleep(SYSTEM_UI_SLEEP_MICROSEC);
		ret = sceImeKeyboardOpen(SCE_USER_SERVICE_USER_ID_EVERYONE, &param);
	}

	if (ret == SCE_OK)
	{
		isActive = true;
	}
	else
	{
		printf("ERROR: Couldn't open keyboard: 0x%08X\n", ret);
	}
}

void Keyboard::Terminate()
{
	isActive = false;
	sceSysmoduleUnloadModule(SCE_SYSMODULE_LIBIME);

	for (auto i = 0; i < PLAYER_MAX; i++)
		Allocator::Get()->release(states[i]);
}

void Keyboard::Restart()
{
	sceImeKeyboardClose(SCE_USER_SERVICE_USER_ID_EVERYONE);
	Initialize();
}

void Keyboard::Update()
{
	if (!isActive)
		return;

	auto ret = sceImeUpdate(HandleKeyboard);
	if (ret == SCE_IME_ERROR_CONNECTION_FAILED)
	{
		// This semi-fatal error occurs when plugging in the keyboard and then
		// selecting a user. It doesn't appear to be documented, but handling
		// it similarly to the abort message appears to work.
		isActive = false;
		Restart();
	}
	else if (ret != SCE_OK)
	{
		printf("ERROR: Couldn't update keyboard: 0x%08X\n", ret);
		isActive = false;
	}
}

int Keyboard::Enable(SceUserServiceUserId userId)
{
	// Is this user already enabled?
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		if (users[i] == userId)
			return i;
	}

	auto playerIndex = findOpenSlot();
	if (playerIndex >= 0)
	{
		users[playerIndex] = userId;
		memset(states[playerIndex], 0, sizeof(KeyboardState));
	}

	return playerIndex;
}

int Keyboard::Disable(SceUserServiceUserId userId)
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		if (users[i] != userId)
			continue;

		users[i] = -1;
		memset(states[i], 0, sizeof(KeyboardState));
		return i;
	}

	return -1;
}

KeyboardState* Keyboard::GetState(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);

	return states[playerIndex];
}
