#include "game.h"

#include <cstdio>

collect_function Game::_collect = NULL;

void Game::SetManagedCollect(collect_function func)
{
	_collect = func;
}

void Game::DoManagedCollect()
{
	if (_collect != NULL)
		_collect();
}
