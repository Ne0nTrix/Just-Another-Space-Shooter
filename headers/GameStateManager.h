#pragma once
#include "GameState.h"

class GameStateManager {
private:
    GameState currentState;
    GameState previousState;
    bool justTransitioned;

public:
    GameStateManager(GameState initialState = GameState::MAIN_MENU);
    GameState getCurrentState() const;
    GameState getPreviousState() const;
    void setCurrentState(GameState newState);
    bool isState(GameState state) const;
    bool hasJustTransitioned() const;
    void markTransitionHandled();
};

