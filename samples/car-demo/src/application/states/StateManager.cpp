
#include "StateManager.hpp"

#include "application/defines.hpp"

#include "State_Paused.hpp"
#include "State_Running.hpp"

#include "geronimo/system/TraceLogger.hpp"

//
//
// singleton

StateManager* StateManager::_instance = nullptr;

StateManager::StateManager() {
  // allocate states

  _states[gero::asValue(States::Running)] = std::make_unique<State_Running>();
  _states[gero::asValue(States::Paused)] = std::make_unique<State_Paused>();

  _currentState = States::Running;

  _previousState = _currentState;
  _states[gero::asValue(_currentState)]->enter();
}

//

void
StateManager::create() {
  if (!_instance)
    _instance = new StateManager();
}

void
StateManager::destroy() {
  delete _instance, _instance = nullptr;
}

StateManager*
StateManager::get() {
  return _instance;
}

// singleton
//
//

void
StateManager::changeState(States nextState) {
  _states[gero::asValue(_currentState)]->leave();

  _previousState = _currentState;
  _currentState = nextState;

  _states[gero::asValue(_currentState)]->enter();
}

void
StateManager::returnToPreviousState() {
  if (_currentState == _previousState)
    return;

  _states[gero::asValue(_currentState)]->leave();

  _currentState = _previousState;

  _states[gero::asValue(_currentState)]->enter();
}

StateManager::States
StateManager::getState() const {
  return _currentState;
}

void
StateManager::handleEvent(const SDL_Event& event) {
  _states[gero::asValue(_currentState)]->handleEvent(event);
}

void
StateManager::update(uint32_t delta) {
  _states[gero::asValue(_currentState)]->update(delta);
}

void
StateManager::render(const SDL_Window& window) {
  _states[gero::asValue(_currentState)]->render(window);
}

void
StateManager::resize(uint32_t width, uint32_t height) {
  _states[gero::asValue(_currentState)]->resize(width, height);
}

void
StateManager::visibility(bool visible) {
  _states[gero::asValue(_currentState)]->visibility(visible);
}
