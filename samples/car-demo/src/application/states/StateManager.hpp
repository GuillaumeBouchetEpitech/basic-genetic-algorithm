
#pragma once

#include "application/defines.hpp"

#include "IState.hpp"

#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/asValue.hpp"

#include <array>
#include <memory>

class StateManager : public gero::NonCopyable {
  //
  //
  // singleton

private:
  static StateManager* _instance;

  StateManager();
  ~StateManager() = default;

public:
  static void create();
  static void destroy();
  static StateManager* get();

  // singleton
  //
  //

public:
  enum class States : std::size_t {
    Running = 0,
    Paused,

    Total
  };

private:
  using StateInstances = std::array<std::unique_ptr<IState>, gero::asValue(States::Total)>;
  StateInstances _states;

  States _currentState;
  States _previousState;

public:
  void changeState(States nextState);
  void returnToPreviousState();
  States getState() const;

public:
  void handleEvent(const SDL_Event&);
  void update(uint32_t);
  void render(const SDL_Window&);
  void resize(uint32_t, uint32_t);
  void visibility(bool);
};
