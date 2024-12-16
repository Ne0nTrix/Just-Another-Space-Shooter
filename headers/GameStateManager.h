#pragma once
#include "GameState.h"
class GameStateManager
{
	GameState currentState;
public:
	GameStateManager(GameState initialState = GameState::MAIN_MENU);
	GameState getCurrentState() const;
	void setCurrentState(GameState newState);
	bool isState(GameState state) const;
};

