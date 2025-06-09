#include "GameStateManager.h"

GameStateManager::GameStateManager(GameState initialState)
    : currentState(initialState), previousState(initialState), justTransitioned(false) {}

GameState GameStateManager::getCurrentState() const {
    return currentState;
}

GameState GameStateManager::getPreviousState() const {
    return previousState;
}

void GameStateManager::setCurrentState(GameState newState) {
    if (currentState != newState) {
        previousState = currentState;
        currentState = newState;
        justTransitioned = true;
    }
}

bool GameStateManager::isState(GameState state) const {
    return currentState == state;
}

bool GameStateManager::hasJustTransitioned() const {
    return justTransitioned;
}

void GameStateManager::markTransitionHandled() {
    justTransitioned = false;
}
