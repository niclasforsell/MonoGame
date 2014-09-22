#include "imeDialog.h"
#include <libsysmodule.h>
#include <wchar.h>
#include <assert.h>
#include <sys/param.h>

using namespace System;

ImeDialog::ImeDialog()
{
	sceImeDialogParamInit(&_params);
	_params.maxTextLength = SCE_IME_MAX_TEXT_LENGTH;

	memset(&_inputText, 0, sizeof(_inputText));
	memset(&_inputTextW, 0, sizeof(_inputTextW));
	_params.inputTextBuffer = _inputTextW;

	memset(&_placeholder, 0, sizeof(_placeholder));
	memset(&_placeholderW, 0, sizeof(_placeholderW));
	_params.placeholder = _placeholderW;

	memset(&_title, 0, sizeof(_title));
	memset(&_titleW, 0, sizeof(_titleW));
	_params.title = _titleW;
}

ImeDialog::~ImeDialog()
{
	Term();
}

ImeError ImeDialog::Init()
{
	sceSysmoduleLoadModule(SCE_SYSMODULE_IME_DIALOG);
	auto err = sceImeDialogInit(&_params, NULL);
	return (ImeError)err;
}

ImeError ImeDialog::Abort()
{
	auto err = sceImeDialogAbort();
	return (ImeError)err;
}

ImeError ImeDialog::Term()
{
	auto err = sceImeDialogTerm();
	return (ImeError)err;
}

ImeDialogStatus ImeDialog::GetStatus()
{
	auto status = sceImeDialogGetStatus();
	return (ImeDialogStatus)status;
}

ImeError ImeDialog::GetResult(CS_OUT ImeDialogEndStatus* status)
{
	SceImeDialogResult result;
	memset(&result, 0, sizeof(result));
	auto err = sceImeDialogGetResult(&result);
	*status = (ImeDialogEndStatus)result.endstatus;
	return (ImeError)err;
}

void ImeDialog::SetUserId(SceUserServiceUserId userId)
{
	_params.userId = userId;
}

void ImeDialog::SetDialogType(ImeDialogType type)
{
	_params.type = (SceImeType)type;
}

ImeDialogType ImeDialog::GetDialogType()
{
	return (ImeDialogType)_params.type;
}

void ImeDialog::SetSupportedLanguages(ImeLanguage lang)
{
	_params.supportedLanguages = (uint64_t)lang;
}

ImeLanguage ImeDialog::GetSupportedLanguages()
{
	return (ImeLanguage)_params.supportedLanguages;
}

void ImeDialog::SetEnterLabel(ImeEnterLabel label)
{
	_params.enterLabel = (SceImeEnterLabel)label;
}

ImeEnterLabel ImeDialog::GetEnterLabel()
{
	return (ImeEnterLabel)_params.enterLabel;
}

void ImeDialog::SetOptions(ImeOption options)
{
	_params.option = (uint32_t)options;
}

ImeOption ImeDialog::GetOptions()
{
	return (ImeOption)_params.option;
}

void ImeDialog::SetMaxTextLength(uint32_t length)
{
	_params.maxTextLength = length;
}

uint32_t ImeDialog::GetMaxTextLength()
{
	return _params.maxTextLength;
}

void ImeDialog::SetPositionX(float x)
{
	_params.posx = x;
}

float ImeDialog::GetPositionX()
{
	return _params.posx;
}

void ImeDialog::SetPositionY(float y)
{
	_params.posy = y;
}

float ImeDialog::GetPositionY()
{
	return _params.posy;
}

void ImeDialog::SetHorizontalAlignment(ImeHorizontalAlignment align)
{
	_params.horizontalAlignment = (SceImeHorizontalAlignment)align;
}

ImeHorizontalAlignment ImeDialog::GetHorizontalAlignment()
{
	return (ImeHorizontalAlignment)_params.horizontalAlignment;
}

void ImeDialog::SetVerticalAlignment(ImeVerticalAlignment align)
{
	_params.verticalAlignment = (SceImeVerticalAlignment)align;
}

ImeVerticalAlignment ImeDialog::GetVerticalAlignment()
{
	return (ImeVerticalAlignment)_params.verticalAlignment;
}

void ImeDialog::SetPlaceholder(const char* text)
{
	auto len = MIN(strlen(text), SCE_IME_DIALOG_MAX_PLACEHOLDER_LENGTH);
	strncpy(_placeholder, text, len);	
	mbstowcs(_placeholderW, _placeholder, len);
}

const char* ImeDialog::GetPlaceholder()
{
	return _placeholder;
}

void ImeDialog::SetTitle(const char* text)
{
	auto len = MIN(strlen(text), SCE_IME_DIALOG_MAX_TITLE_LENGTH);
	strncpy(_title, text, len);	
	mbstowcs(_titleW, _title, len);
}

const char* ImeDialog::GetTitle()
{
	return _title;
}

const char* ImeDialog::GetInputText()
{
	wcstombs(_inputText, _inputTextW, wcslen(_inputTextW));
	return _inputText;
}
