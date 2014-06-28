#include "keyboard.h"
#include "keyboardState.h"
#include "../System/userService.h"
#include "vkeys.h"

#include <cstdio>
#include <assert.h>
#include <sceerror.h>
#include <kernel.h>
#include <libsysmodule.h>
#include <libime.h>
#include <system_service.h>

using namespace Input;

namespace {
	const int PLAYER_MAX = 4;
	const int SYSTEM_UI_SLEEP_MICROSEC = 5000;

	KeyboardState states[PLAYER_MAX];

	void HandleKeyboard(void* arg, const SceImeEvent* e)
	{
		auto usbKeyCode = (uint8_t)e->param.keycode.keycode;
		auto virtKeyCode = toVKcode[usbKeyCode];
		auto mask = (uint32_t)1 << (((uint32_t)virtKeyCode) & 0x1f);
		auto keys = (uint32_t)virtKeyCode >> 5;

		switch(e->id)
		{
		case SCE_IME_KEYBOARD_EVENT_ABORT:
			// Per the sceImeKeyboardOpen docs, an abort event can occur immediately
			// after calling open, in which case we should close and re-open.
			break;

		case SCE_IME_KEYBOARD_EVENT_DISCONNECTION:
			{
				auto userId = e->param.resourceIdArray.userId;
				auto playerIndex = System::UserService::GetPlayerIndexByUserId(userId);
				if (playerIndex < 0)
				{
					printf("WARNING: User %d not found for keyboard disconnect event.\n", userId);
					return;
				}

				// Clear the keyboard state for the one player.
				assert(playerIndex < PLAYER_MAX);
				memset(&states[playerIndex], 0, sizeof(KeyboardState));
			}
			break;

		case SCE_IME_KEYBOARD_EVENT_KEYCODE_DOWN:
			{
				auto userId = e->param.keycode.userId;
				auto playerIndex = System::UserService::GetPlayerIndexByUserId(userId);
				if (playerIndex < 0)
				{
					printf("WARNING: User %d not found for keydown event.\n", userId);
					return;
				}

				assert(playerIndex < PLAYER_MAX);
				auto state = &states[playerIndex];
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
				auto userId = e->param.keycode.userId;
				auto playerIndex = System::UserService::GetPlayerIndexByUserId(userId);
				if (playerIndex < 0)
				{
					printf("WARNING: User %d not found for keyup event.\n", userId);
					return;
				}

				assert(playerIndex < PLAYER_MAX);
				auto state = &states[playerIndex];
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

		default:
			break;
		}
	}
}

void Keyboard::Initialize()
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_LIBIME) != SCE_OK)
	{
		memset(states, 0, sizeof(KeyboardState) * PLAYER_MAX);

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
	if (ret == SCE_IME_ERROR_NOT_ACTIVE)
	{
		// This can happen... don't worry we'll 
		// retry on the next update!
	}
	else if (ret != SCE_OK)
	{
		printf("ERROR: Couldn't open keyboard: 0x%08X\n", ret);
	}
}

void Keyboard::Terminate()
{
	sceImeKeyboardClose(SCE_USER_SERVICE_USER_ID_EVERYONE);
	sceSysmoduleUnloadModule(SCE_SYSMODULE_LIBIME);
	memset(states, 0, sizeof(KeyboardState) * PLAYER_MAX);
}

void Keyboard::Update()
{
	auto ret = sceImeUpdate(HandleKeyboard);
	if (ret == SCE_IME_ERROR_NOT_OPENED)
	{
		// Try initializing again for the next frame.
		Initialize();
	}
	else if (ret == SCE_IME_ERROR_CONNECTION_FAILED)
	{
		// This happens when we loose focus, just
		// close the connection and retry later.
		sceImeKeyboardClose(SCE_USER_SERVICE_USER_ID_EVERYONE);
		memset(states, 0, sizeof(KeyboardState) * PLAYER_MAX);
	}
	else if (ret != SCE_OK)
	{
		printf("WARNING: Couldn't update keyboard: 0x%08X\n", ret);
	}
}

int Keyboard::Enable(SceUserServiceUserId userId, int playerIndex)
{
	memset(&states[playerIndex], 0, sizeof(KeyboardState));
	return 0;
}

int Keyboard::Disable(int playerIndex)
{
	memset(&states[playerIndex], 0, sizeof(KeyboardState));
	return 0;
}

KeyboardState* Keyboard::GetState(int playerIndex)
{
	assert(playerIndex >= 0);
	assert(playerIndex < PLAYER_MAX);
	return &states[playerIndex];
}
