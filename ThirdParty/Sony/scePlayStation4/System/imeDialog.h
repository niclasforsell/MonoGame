#pragma once

#include "predecls.h"

#include "commonDialog.h"

#include <ime_dialog.h>
#include <sceerror.h>

namespace System {


enum class ImeError
{
	Ok = SCE_OK,

	Busy = SCE_IME_ERROR_BUSY,
	NotOpened = SCE_IME_ERROR_NOT_OPENED,
	NoMemory = SCE_IME_ERROR_NO_MEMORY,
	ConnectionFailed = SCE_IME_ERROR_CONNECTION_FAILED,
	TooManyRequests = SCE_IME_ERROR_TOO_MANY_REQUESTS,
	InvalidText = SCE_IME_ERROR_INVALID_TEXT,
	EventOverflow = SCE_IME_ERROR_EVENT_OVERFLOW,
	NotActive = SCE_IME_ERROR_NOT_ACTIVE,
	ImeSuspending = SCE_IME_ERROR_IME_SUSPENDING,
	DeviceInUse = SCE_IME_ERROR_DEVICE_IN_USE,
	InvalidUserId = SCE_IME_ERROR_INVALID_USER_ID,
	InvalidType = SCE_IME_ERROR_INVALID_TYPE,
	InvalidSupportedLanguages = SCE_IME_ERROR_INVALID_SUPPORTED_LANGUAGES,
	InvalidEnterLabel = SCE_IME_ERROR_INVALID_ENTER_LABEL,
	InvalidInputMethod = SCE_IME_ERROR_INVALID_INPUT_METHOD,
	InvalidOption = SCE_IME_ERROR_INVALID_OPTION,
	InvalidMaxTextLength = SCE_IME_ERROR_INVALID_MAX_TEXT_LENGTH,
	InvalidInputTextBuffer = SCE_IME_ERROR_INVALID_INPUT_TEXT_BUFFER,
	InvalidPosX = SCE_IME_ERROR_INVALID_POSX,
	InvalidPosY = SCE_IME_ERROR_INVALID_POSY,
	InvalidHorizontalAlignment = SCE_IME_ERROR_INVALID_HORIZONTAL_ALIGNMENT,
	InvalidVerticalAlignment = SCE_IME_ERROR_INVALID_VERTICAL_ALIGNMENT,
	InvalidExtended = SCE_IME_ERROR_INVALID_EXTENDED,
	InvalidKeyboardType = SCE_IME_ERROR_INVALID_KEYBOARD_TYPE,
	InvalidWork = SCE_IME_ERROR_INVALID_WORK,
	InvalidArg = SCE_IME_ERROR_INVALID_ARG,
	InvalidHandler = SCE_IME_ERROR_INVALID_HANDLER,
	InvalidNoResourceId = SCE_IME_ERROR_NO_RESOURCE_ID,
	InvalidMode = SCE_IME_ERROR_INVALID_MODE,
	InvalidParam = SCE_IME_ERROR_INVALID_PARAM,
	InvalidAddress = SCE_IME_ERROR_INVALID_ADDRESS,
	InvalidReserved = SCE_IME_ERROR_INVALID_RESERVED,
	InvalidTiming = SCE_IME_ERROR_INVALID_TIMING,
	InvalidInternal = SCE_IME_ERROR_INTERNAL,
};

enum class ImeDialogStatus
{
	None = SCE_IME_DIALOG_STATUS_NONE,
	Running = SCE_IME_DIALOG_STATUS_RUNNING,
	Finished = SCE_IME_DIALOG_STATUS_FINISHED,
};

enum class ImeDialogEndStatus
{
	Ok = SCE_IME_DIALOG_END_STATUS_OK,
	Canceled = SCE_IME_DIALOG_END_STATUS_USER_CANCELED,
	Aborted = SCE_IME_DIALOG_END_STATUS_ABORTED,
};

enum class ImeDialogType
{
	Default = SCE_IME_TYPE_DEFAULT,
	BasicLatin = SCE_IME_TYPE_BASIC_LATIN,
	Url = SCE_IME_TYPE_URL,
	Main = SCE_IME_TYPE_MAIL,
	Number = SCE_IME_TYPE_NUMBER,
};

CS_FLAGS enum class ImeLanguage : uint64_t
{
	All = 0,
	Danish = SCE_IME_LANGUAGE_DANISH,
	German = SCE_IME_LANGUAGE_GERMAN,
	EnglishUS = SCE_IME_LANGUAGE_ENGLISH_US,
	EnglishGB = SCE_IME_LANGUAGE_ENGLISH_GB,
	Spanish = SCE_IME_LANGUAGE_SPANISH,
	SpanishLA = SCE_IME_LANGUAGE_SPANISH_LA,
	French = SCE_IME_LANGUAGE_FRENCH,
	Italian = SCE_IME_LANGUAGE_ITALIAN,
	Dutch = SCE_IME_LANGUAGE_DUTCH,
	Norwegian = SCE_IME_LANGUAGE_NORWEGIAN,
	Polish = SCE_IME_LANGUAGE_POLISH,
	PortuguesePT = SCE_IME_LANGUAGE_PORTUGUESE_PT,
	PortugueseBR = SCE_IME_LANGUAGE_PORTUGUESE_BR,
	Russian = SCE_IME_LANGUAGE_RUSSIAN,
	Finnish = SCE_IME_LANGUAGE_FINNISH,
	Swedish = SCE_IME_LANGUAGE_SWEDISH,
	Japanese = SCE_IME_LANGUAGE_JAPANESE,
	Korean = SCE_IME_LANGUAGE_KOREAN,
	ChineseSimplified = SCE_IME_LANGUAGE_SIMPLIFIED_CHINESE,
	ChineseTraditional = SCE_IME_LANGUAGE_TRADITIONAL_CHINESE,
	Turkish = SCE_IME_LANGUAGE_TURKISH,
};

enum class ImeEnterLabel
{
	Default = SCE_IME_ENTER_LABEL_DEFAULT,
	Send = SCE_IME_ENTER_LABEL_SEND,
	Search = SCE_IME_ENTER_LABEL_SEARCH,
	Go = SCE_IME_ENTER_LABEL_GO,
};

CS_FLAGS enum class ImeOption
{
	Default = SCE_IME_OPTION_DEFAULT,
	Multiline = SCE_IME_OPTION_MULTILINE,
	NoAutoCapitalization = SCE_IME_OPTION_NO_AUTO_CAPITALIZATION,
	Password = SCE_IME_OPTION_PASSWORD,
	LanguagesForced = SCE_IME_OPTION_LANGUAGES_FORCED,
	ExtKeyboard = SCE_IME_OPTION_EXT_KEYBOARD,
	NoLearning = SCE_IME_OPTION_NO_LEARNING,
	FixedPosition = SCE_IME_OPTION_FIXED_POSITION,
	DisableCopyPaste = SCE_IME_OPTION_DISABLE_COPY_PASTE,
	DisableResume = SCE_IME_OPTION_DISABLE_RESUME,
};

enum class ImeHorizontalAlignment 
{
	Left = SCE_IME_HALIGN_LEFT,
	Center = SCE_IME_HALIGN_CENTER,
	Right = SCE_IME_HALIGN_RIGHT,
};

enum class ImeVerticalAlignment  
{
	Top = SCE_IME_VALIGN_TOP,
	Center = SCE_IME_VALIGN_CENTER,
	Bottom = SCE_IME_VALIGN_BOTTOM,
};


class CS_API ImeDialog
{
private:

	wchar_t _inputTextW[SCE_IME_MAX_TEXT_LENGTH+1];
	wchar_t _placeholderW[SCE_IME_DIALOG_MAX_PLACEHOLDER_LENGTH+1];
	wchar_t _titleW[SCE_IME_DIALOG_MAX_TITLE_LENGTH+1];

	// HACK FOR CPPSHARP NOT SUPPORTING wchar_t!
	char _inputText[SCE_IME_MAX_TEXT_LENGTH+1];
	char _placeholder[SCE_IME_DIALOG_MAX_PLACEHOLDER_LENGTH+1];
	char _title[SCE_IME_DIALOG_MAX_TITLE_LENGTH+1];

	SceImeDialogParam _params;

public:

	ImeDialog();
	~ImeDialog();

	ImeError Init();

	ImeError Abort();

	ImeError Term();
	
	ImeDialogStatus GetStatus();

	ImeError GetResult(CS_OUT ImeDialogEndStatus* result);

	void SetUserId(SceUserServiceUserId userId);

	void SetDialogType(ImeDialogType type);
	ImeDialogType GetDialogType();

	void SetSupportedLanguages(ImeLanguage lang);
	ImeLanguage GetSupportedLanguages();

	void SetEnterLabel(ImeEnterLabel label);
	ImeEnterLabel GetEnterLabel();

	void SetOptions(ImeOption options);
	ImeOption GetOptions();

	void SetMaxTextLength(uint32_t length);
	uint32_t GetMaxTextLength();

	void SetPositionX(float x);
	float GetPositionX();

	void SetPositionY(float y);
	float GetPositionY();

	void SetHorizontalAlignment(ImeHorizontalAlignment align);
	ImeHorizontalAlignment GetHorizontalAlignment();

	void SetVerticalAlignment(ImeVerticalAlignment align);
	ImeVerticalAlignment GetVerticalAlignment();

	void SetPlaceholder(const char *text);
	const char* GetPlaceholder();

	void SetTitle(const char *text);
	const char* GetTitle();

	const char* GetInputText();
};

} // namespace System
