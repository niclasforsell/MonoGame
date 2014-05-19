#pragma once

typedef void (*collect_function)(void);

class __declspec(dllexport) Game
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

