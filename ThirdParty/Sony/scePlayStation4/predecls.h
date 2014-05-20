#pragma once
#include <stdint.h>

#define CS_IGNORE
#define CS_OUT
#define CS_VALUE_TYPE
#define CS_FLAGS

#if defined(_MSC_VER) && !defined(__clang__)
#define CS_API __declspec(dllexport)
#else
#define CS_API __declspec(dllexport)
#endif
