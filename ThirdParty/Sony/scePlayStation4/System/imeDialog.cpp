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
	_params.inputTextBuffer = _inputText;

	memset(&_placeholder, 0, sizeof(_placeholder));
	_params.placeholder = _placeholder;

	memset(&_title, 0, sizeof(_title));
	_params.title = _title;
}

ImeDialog::~ImeDialog()
{
}

ImeError ImeDialog::Open()
{
	auto err = sceImeDialogInit(&_params, NULL);
	return (ImeError)err;
}

ImeError ImeDialog::Abort()
{
	auto err = sceImeDialogAbort();
	return (ImeError)err;
}

ImeError ImeDialog::Close()
{
	auto err = sceImeDialogTerm();
	return (ImeError)err;
}

ImeDialogStatus ImeDialog::GetStatus()
{
	auto status = sceImeDialogGetStatus();
	return (ImeDialogStatus)status;
}

ImeError ImeDialog::GetResult(CS_OUT ImeDialogEndStatus* result)
{
	auto err = sceImeDialogGetResult((SceImeDialogResult*)result);
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

void ImeDialog::SetPlaceholder(const wchar_t* text)
{
	auto len = MIN(wcslen(text), SCE_IME_DIALOG_MAX_PLACEHOLDER_LENGTH);
	wcsncpy(_placeholder, text, len);	
}

const wchar_t* ImeDialog::GetPlaceholder()
{
	return _placeholder;
}

void ImeDialog::SetTitle(const wchar_t* text)
{
	auto len = MIN(wcslen(text), SCE_IME_DIALOG_MAX_TITLE_LENGTH);
	wcsncpy(_title, text, len);	
}

const wchar_t* ImeDialog::GetTitle()
{
	return _title;
}

const wchar_t* ImeDialog::GetInputText()
{
	return _inputText;
}
