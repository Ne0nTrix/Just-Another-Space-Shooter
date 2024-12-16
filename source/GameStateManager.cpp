#include "GameStateManager.h"

GameStateManager::GameStateManager(GameState initialState)
: currentState(initialState) {}

GameState GameStateManager::getCurrentState() const
{
	return currentState;
}

void GameStateManager::setCurrentState(GameState newState)
{
	currentState = newState;
}

bool GameStateManager::isState(GameState state) const
{
	return currentState == state;
}
