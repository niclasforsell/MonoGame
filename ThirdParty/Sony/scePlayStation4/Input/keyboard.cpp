#include "keyboard.h"
#include "keyboardState.h"
#include "../allocator.h"

#include <cstdio>
#include <assert.h>
#include <sceerror.h>
#include <libsysmodule.h>
#include <libime.h>

using namespace Input;

namespace {
	const int PLAYER_MAX = 4;

	KeyboardState* states[PLAYER_MAX];
	SceUserServiceUserId users[PLAYER_MAX];

	uint8_t toVKcode[256] = {
		0,      // SCE_IME_KEYCODE_NOEVENT
		0,      // SCE_IME_KEYCODE_ERRORROLLOVER
		0,      // SCE_IME_KEYCODE_POSTFAIL
		0,      // SCE_IME_KEYCODE_ERRORUNDEFINED
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
		'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
		0x0D,   // SCE_IME_KEYCODE_RETURN         -> VK_RETURN
		0x1B,   // SCE_IME_KEYCODE_ESCAPE         -> VK_ESCAPE
		0x08,   // SCE_IME_KEYCODE_BACKSPACE      -> VK_BACK
		0x09,   // SCE_IME_KEYCODE_TAB            -> VK_TAB
		0x20,   // SCE_IME_KEYCODE_SPACEBAR       -> VK_SPACE
		0xBD,   // SCE_IME_KEYCODE_MINUS          -> VK_OEM_MINUS
		0xBB,   // SCE_IME_KEYCODE_EQUAL          -> VK_OEM_PLUS
		0xDB,   // SCE_IME_KEYCODE-LEFTBRACKET    -> VK_OEM_4
		0xDD,   // SCE_IME_KEYCODE_RIGHTBRACKET   -> VK_OEM_6
		0xDC,   // SCE_IME_KEYCODE_BACKSLASH      -> VK_OEM_5
		'3',    // SCE_IME_KEYCODE_NONUS_POUND    -> VK_KEY_3
		0xBA,   // SCE_IME_KEYCODE_SEMICOLON      -> VK_OEM_1
		0xDE,   // SCE_IME_KEYCODE_SINGLEQUOTE    -> VK_OEM_7
		0xC0,   // SCE_IME_KEYCODE_BACKQUOTE      -> VK_OEM_3
		0xBC,   // SCE_IME_KEYCODE_COMMA          -> VK_OEM_COMMA
		0xBE,   // SCE_IME_KEYCODE_PERIOD         -> VK_OEM_PERIOD
		0xBF,   // SCE_IME_KEYCODE_SLASH          -> VK_OEM_2
		0x14,   // SCE_IME_KEYCODE_CAPSLOCK       -> VK_CAPITAL
		0x70,   // SCE_IME_KEYCODE_F1             -> VK_F1
		0x71,   // SCE_IME_KEYCODE_F2             -> VK_F2
		0x72,   // SCE_IME_KEYCODE_F3             -> VK_F3
		0x73,   // SCE_IME_KEYCODE_F4             -> VK_F4
		0x74,   // SCE_IME_KEYCODE_F5             -> VK_F5
		0x75,   // SCE_IME_KEYCODE_F6             -> VK_F6
		0x76,   // SCE_IME_KEYCODE_F7             -> VK_F7
		0x77,   // SCE_IME_KEYCODE_F8             -> VK_F8
		0x78,   // SCE_IME_KEYCODE_F9             -> VK_F9
		0x79,   // SCE_IME_KEYCODE_F10            -> VK_F10
		0x7A,   // SCE_IME_KEYCODE_F11            -> VK_F11
		0x7B,   // SCE_IME_KEYCODE_F12            -> VK_F12
		0x2A,   // SCE_IME_KEYCODE_PRINTSCREEN    -> VK_PRINT
		0x91,   // SCE_IME_KEYCODE_SCROLLLOCK     -> VK_SCROLL
		0x13,   // SCE_IME_KEYCODE_PAUSE          -> VK_PAUSE
		0x2D,   // SCE_IME_KEYCODE_INSERT         -> VK_INSERT
		0x24,   // SCE_IME_KEYCODE_HOME           -> VK_HOME
		0x21,   // SCE_IME_KEYCODE_PAGEUP         -> VK_PRIOR
		0x2E,   // SCE_IME_KEYCODE_DELETE         -> VK_DELETE
		0x23,   // SCE_IME_KEYCODE_END            -> VK_END
		0x22,   // SCE_IME_KEYCODE_PAGEDOWN       -> VK_NEXT
		0x27,   // SCE_IME_KEYCODE_RIGHTARROW     -> VK_RIGHT
		0x25,   // SCE_IME_KEYCODE_LEFTARROW      -> VK_LEFT
		0x28,   // SCE_IME_KEYCODE_DOWNARROW      -> VK_DOWN
		0x26,   // SCE_IME_KEYCODE_UPARROW        -> VK_UP
		0x90,   // SCE_IME_KEYCODE_KEYPAD_NUMLOCK -> VK_NUMLOCK
		0x6F,   // SCE_IME_KEYCODE_KEYPAD_SLASH   -> VK_DIVIDE
		0x6A,   // SCE_IME_KEYCODE_KEYPAD_ASTERISK -> VK_MULTIPLY
		0x6D,   // SCE_IME_KEYCODE_KEYPAD_MINUS   -> VK_SUBTRACT
		0x6B,   // SCE_IME_KEYCODE_KEYPAD_PLUS    -> VK_ADD
		0x0D,   // SCE_IME_KEYCODE_KEYPAD_ENTER   -> VK_RETURN
		0x61,   // SCE_IME_KEYCODE_KEYPAD_1       -> VK_NUMPAD1
		0x62,   // SCE_IME_KEYCODE_KEYPAD_2       -> VK_NUMPAD2
		0x63,   // SCE_IME_KEYCODE_KEYPAD_3       -> VK_NUMPAD3
		0x64,   // SCE_IME_KEYCODE_KEYPAD_4       -> VK_NUMPAD4
		0x65,   // SCE_IME_KEYCODE_KEYPAD_5       -> VK_NUMPAD5
		0x66,   // SCE_IME_KEYCODE_KEYPAD_6       -> VK_NUMPAD6
		0x67,   // SCE_IME_KEYCODE_KEYPAD_7       -> VK_NUMPAD7
		0x68,   // SCE_IME_KEYCODE_KEYPAD_8       -> VK_NUMPAD8
		0x69,   // SCE_IME_KEYCODE_KEYPAD_9       -> VK_NUMPAD9
		0x60,   // SCE_IME_KEYCODE_KEYPAD_0       -> VK_NUMPAD0
		0xBE,   // SCE_IME_KEYCODE_KEYPAD_PERIOD  -> VK_OEM_PERIOD
		0xDC,   // SCE_IME_KEYCODE_NONUS_BACKSLASH -> VK_OEM_5
		0x5D,   // SCE_IME_KEYCODE_APPLICATION    -> VK_APPS
		0,      // SCE_IME_KEYCODE_POWER
		0,      // SCE_IME_KEYCODE_KEYPAD_EQUAL
		0x7C,   // SCE_IME_KEYCODE_F13          -> VK_F13
		0x7D,   // SCE_IME_KEYCODE_F14          -> VK_F14
		0x7E,   // SCE_IME_KEYCODE_F15          -> VK_F15
		0x7F,   // SCE_IME_KEYCODE_F16          -> VK_F16
		0x80,   // SCE_IME_KEYCODE_F17          -> VK_F17
		0x81,   // SCE_IME_KEYCODE_F18          -> VK_F18
		0x82,   // SCE_IME_KEYCODE_F19          -> VK_F19
		0x83,   // SCE_IME_KEYCODE_F20          -> VK_F20
		0x84,   // SCE_IME_KEYCODE_F21          -> VK_F21
		0x85,   // SCE_IME_KEYCODE_F22          -> VK_F22
		0x86,   // SCE_IME_KEYCODE_F23          -> VK_F23
		0x87,   // SCE_IME_KEYCODE_F24          -> VK_F24
		0x2B,   // SCE_IME_KEYCODE_EXECUTE      -> VK_EXECUTE
		0x2F,   // SCE_IME_KEYCODE_HELP         -> VK_HELP
		0,      // SCE_IME_KEYCODE_MENU
		0x29,   // SCE_IME_KEYCODE_SELECT       -> VK_SELECT
		0xB2,   // SCE_IME_KEYCODE_STOP         -> VK_MEDIA_STOP
		0,      // SCE_IME_KEYCODE_AGAIN
		0,      // SCE_IME_KEYCODE_UNDO
		0,      // SCE_IME_KEYCODE_CUT
		0,      // SCE_IME_KEYCODE_COPY
		0,      // SCE_IME_KEYCODE_PASTE
		0,      // SCE_IME_KEYCODE_FIND
		0xAD,   // SCE_IME_KEYCODE_MUTE         -> VK_VOLUME_MUTE
		0xAF,   // SCE_IME_KEYCODE_VOLUMEUP     -> VK_VOLUME_UP
		0xAE,   // SCE_IME_KEYCODE_VOLUMEDOWN   -> VK_VOLUME_DOWN
		0x14,   // SCE_IME_KEYCODE_LOCKING_CAPSLOCK     -> VK_CAPITAL
		0x90,   // SCE_IME_KEYCODE_LOCKING_NUMLOCK      -> VK_NUMLOCK
		0x91,   // SCE_IME_KEYCODE_LOCKING_SCROLLLOCK   -> VK_SCROLL
		0xBC,   // SCE_IME_KEYCODE_KEYPAD_COMMA         -> VK_OEM_COMMA
		0,      // SCE_IME_KEYCODE_KEYPAD_EQUALSIGN
		0,      // SCE_IME_KEYCODE_INTERNATIONAL1
		0,      // SCE_IME_KEYCODE_INTERNATIONAL2
		0,      // SCE_IME_KEYCODE_INTERNATIONAL3
		0,      // SCE_IME_KEYCODE_INTERNATIONAL4
		0,      // SCE_IME_KEYCODE_INTERNATIONAL5
		0,      // SCE_IME_KEYCODE_INTERNATIONAL6
		0,      // SCE_IME_KEYCODE_INTERNATIONAL7
		0,      // SCE_IME_KEYCODE_INTERNATIONAL8
		0,      // SCE_IME_KEYCODE_INTERNATIONAL9
		0,      // SCE_IME_KEYCODE_LANG1
		0,      // SCE_IME_KEYCODE_LANG2
		0,      // SCE_IME_KEYCODE_LANG3
		0,      // SCE_IME_KEYCODE_LANG4
		0,      // SCE_IME_KEYCODE_LANG5
		0,      // SCE_IME_KEYCODE_LANG6
		0,      // SCE_IME_KEYCODE_LANG7
		0,      // SCE_IME_KEYCODE_LANG8
		0,      // SCE_IME_KEYCODE_LANG9
		0,      // SCE_IME_KEYCODE_ALTERASE
		0x2A,   // SCE_IME_KEYCODE_SYSREQ       -> VK_PRINT
		0x03,   // SCE_IME_KEYCODE_CANCEL       -> VK_CANCEL
		0x0C,   // SCE_IME_KEYCODE_CLEAR        -> VK_CLEAR
		0x21,   // SCE_IME_KEYCODE_PRIOR        -> VK_PRIOR
		0x0D,   // SCE_IME_KEYCODE_RETURN2      -> VK_RETURN
		0x6C,   // SCE_IME_KEYCODE_SEPARATOR    -> VK_SEPARATOR
		0,      // SCE_IME_KEYCODE_OUT
		0,      // SCE_IME_KEYCODE_OPER
		0,      // SCE_IME_KEYCODE_CLEAR_AGAIN
		0xF7,   // SCE_IME_KEYCODE_CRSEL_PROPS  -> VK_CRSEL
		0xF8,   // SCE_IME_KEYCODE_EXSEL        -> VK_EXSEL
		0,      // SCE_IME_KEYCODE_KEYPAD_00
		0,      // SCE_IME_KEYCODE_KEYPAD_000
		0,      // SCE_IME_KEYCODE_THOUSANDSSEPARATOR
		0,      // SCE_IME_KEYCODE_DECIMALSEPARATOR
		0,      // SCE_IME_KEYCODE_CURRENCYUNIT
		0,      // SCE_IME_KEYCODE_CURRENCYSUBUNIT
		0,      // SCE_IME_KEYCODE_KEYPAD_LEFTPARENTHESIS
		0,      // SCE_IME_KEYCODE_KEYPAD_RIGHTPARENTHESIS
		0,      // SCE_IME_KEYCODE_KEYPAD_LEFTCURLYBRACKET
		0,      // SCE_IME_KEYCODE_KEYPAD_RIGHTCURLYBRACKET
		0,      // SCE_IME_KEYCODE_KEYPAD_TAB
		0,      // SCE_IME_KEYCODE_KEYPAD_BACKSPACE
		0,      // SCE_IME_KEYCODE_KEYPAD_A
		0,      // SCE_IME_KEYCODE_KEYPAD_B
		0,      // SCE_IME_KEYCODE_KEYPAD_C
		0,      // SCE_IME_KEYCODE_KEYPAD_D
		0,      // SCE_IME_KEYCODE_KEYPAD_E
		0,      // SCE_IME_KEYCODE_KEYPAD_F
		0,      // SCE_IME_KEYCODE_KEYPAD_XOR
		0,      // SCE_IME_KEYCODE_KEYPAD_HAT
		0,      // SCE_IME_KEYCODE_KEYPAD_PERCENT
		0,      // SCE_IME_KEYCODE_KEYPAD_LESSTHAN
		0,      // SCE_IME_KEYCODE_KEYPAD_GREATERTHAN
		0,      // SCE_IME_KEYCODE_KEYPAD_AND
		0,      // SCE_IME_KEYCODE_KEYPAD_LOGICALAND
		0,      // SCE_IME_KEYCODE_KEYPAD_OR
		0,      // SCE_IME_KEYCODE_KEYPAD_LOGICALOR
		0,      // SCE_IME_KEYCODE_KEYPAD_COLON
		0,      // SCE_IME_KEYCODE_KEYPAD_NUMBER
		0,      // SCE_IME_KEYCODE_KEYPAD_SPACE
		0,      // SCE_IME_KEYCODE_KEYPAD_ATSIGN
		0,      // SCE_IME_KEYCODE_KEYPAD_EXCLAMATION
		0,      // SCE_IME_KEYCODE_KEYPAD_MEMORY_STORE
		0,      // SCE_IME_KEYCODE_KEYPAD_MEMORY_RECALL
		0,      // SCE_IME_KEYCODE_KEYPAD_MEMORY_CLEAR
		0,      // SCE_IME_KEYCODE_KEYPAD_MEMORY_ADD
		0,      // SCE_IME_KEYCODE_KEYPAD_MEMORY_SUBTRACT
		0,      // SCE_IME_KEYCODE_KEYPAD_MEMORY_MULTIPLY
		0,      // SCE_IME_KEYCODE_KEYPAD_MEMORY_DIVIDE
		0,      // SCE_IME_KEYCODE_KEYPAD_PLUS_MINUS
		0,      // SCE_IME_KEYCODE_KEYPAD_CLEAR
		0,      // SCE_IME_KEYCODE_KEYPAD_CLEARENTRY
		0,      // SCE_IME_KEYCODE_KEYPAD_BINARY
		0,      // SCE_IME_KEYCODE_KEYPAD_OCTAL
		0,      // SCE_IME_KEYCODE_KEYPAD_DECIMAL
		0,      // SCE_IME_KEYCODE_KEYPAD_HEXADECIMAL
		0xA2,   // SCE_IME_KEYCODE_LEFTCONTROL  -> VK_LCONTROL
		0xA0,   // SCE_IME_KEYCODE_LEFTSHIFT    -> VK_LSHIFT
		0xA4,   // SCE_IME_KEYCODE_LEFTALT      -> VK_LMENU
		0x5B,   // SCE_IME_KEYCODE_LEFTGUI      -> VK_LWIN
		0xA3,   // SCE_IME_KEYCODE_RIGHTCONTROL -> VK_RCONTROL
		0xA1,   // SCE_IME_KEYCODE_RIGHTSHIFT   -> VK_RSHIFT
		0xA5,   // SCE_IME_KEYCODE_RIGHTALT     -> VK_RMENU
		0x5C,   // SCE_IME_KEYCODE_RIGHTGUI     -> VK_RWIN
	};

	int findOpenSlot()
	{
		for (auto i = 0; i < PLAYER_MAX; i++)
		{
			if (users[i] == -1)
				return i;
		}

		return -1;
	}

	void HandleKeyboard(void* arg, const SceImeEvent* e)
	{
		auto state = (KeyboardState*)arg;
		assert(state != NULL);

		// TODO: Ignore all except e->param.deviceType == SCE_IME_DEVICE_TYPE_EXT_KEYBOARD?

		auto usbKeyCode = (uint8_t)e->param.keycode.keycode;
		auto virtKeyCode = toVKcode[usbKeyCode];
		auto mask = (uint32_t)1 << (((uint32_t)virtKeyCode) & 0x1f);
		auto keys = (uint32_t)virtKeyCode >> 5;

		switch(e->id)
		{
		case SCE_IME_KEYBOARD_EVENT_KEYCODE_DOWN:
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
			break;

		case SCE_IME_KEYBOARD_EVENT_KEYCODE_UP:
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
			break;
		}
	}
}

void Keyboard::Initialize()
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		states[i] = (KeyboardState*)Allocator::Get()->allocate(sizeof(KeyboardState));
		memset(states[i], 0, sizeof(KeyboardState));
		users[i] = -1;
	}

	auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_LIBIME);
	if (ret < 0) {
		printf("ERROR: Couldn't initialize keyboard input library: 0x%08X\n", ret);
	}
}

void Keyboard::Terminate()
{
	sceSysmoduleUnloadModule(SCE_SYSMODULE_LIBIME);

	for (auto i = 0; i < PLAYER_MAX; i++)
		Allocator::Get()->release(states[i]);
}

void Keyboard::Update()
{
	sceImeUpdate(HandleKeyboard);
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
		SceImeKeyboardParam param;
		memset(&param, 0, sizeof(SceImeKeyboardParam));
		param.arg = states[playerIndex];
		param.handler = HandleKeyboard;
		param.option = SCE_IME_KEYBOARD_OPTION_DEFAULT;
		memset(param.reserved1, 0, sizeof(param.reserved1));
		memset(param.reserved2, 0, sizeof(param.reserved2));

		auto ret = sceImeKeyboardOpen(userId, &param);
		if (ret != SCE_OK)
		{
			printf("ERROR: Couldn't open keyboard: 0x%08X\n", ret);
			return ret;
		}

		users[playerIndex] = userId;
	}

	return playerIndex;
}

int Keyboard::Disable(SceUserServiceUserId userId)
{
	for (auto i = 0; i < PLAYER_MAX; i++)
	{
		if (users[i] != userId)
			continue;

		sceImeKeyboardClose(userId);
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
