#include "GameStateManager.h"

GameStateManager::GameStateManager(GameState initialState)
    : currentState(initialState), previousState(initialState) {}

GameState GameStateManager::getCurrentState() const {
    return currentState;
}

GameState GameStateManager::getPreviousState() const {
    return previousState;
}

void GameStateManager::setCurrentState(GameState newState) {
    previousState = currentState;
    currentState = newState;
}

bool GameStateManager::isState(GameState state) const {
    return currentState == state;
}
