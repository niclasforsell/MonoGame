#include "game.h"

#include <system_service.h>

collect_function Game::_collect = NULL;

bool Game::IsActive()
{
	SceSystemServiceStatus status;
	sceSystemServiceGetStatus(&status);
	return !status.isInBackgroundExecution;
}

void Game::HideSplashScreen()
{
	sceSystemServiceHideSplashScreen();
}

void Game::SetManagedCollect(collect_function func)
{
	_collect = func;
}

void Game::DoManagedCollect()
{
	if (_collect != NULL)
		_collect();
}
