#pragma once
#include <stdint.h>

#define CS_IGNORE
#define CS_OUT
#define CS_VALUE_TYPE
#define CS_FLAGS

#if !defined(CS_BINDING_GEN)
#define CS_API __declspec(dllexport)
#else
#define CS_API
#endif
