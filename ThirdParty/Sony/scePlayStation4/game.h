#pragma once
#include "predecls.h"

typedef void (*collect_function)(void);

class CS_API Game
{
private:
	static collect_function _collect;

	Game() {}

public:

	static bool GetIsActive();

	static void HideSplashScreen();
	
	static void SetManagedCollect(collect_function func);
	static void DoManagedCollect();
};

