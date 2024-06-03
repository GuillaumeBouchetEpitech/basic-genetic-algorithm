
#pragma once

#include "geronimo/helpers/SDL.hpp" // SDL_Event + SDL_Window
#include "geronimo/system/NonCopyable.hpp"

class IState : public gero::NonCopyable {
public:
  virtual ~IState(){};

public:
  virtual void enter() = 0;
  virtual void leave() = 0;

public:
  virtual void handleEvent(const SDL_Event&) = 0;
  virtual void update(uint32_t) = 0;
  virtual void render(const SDL_Window&) = 0;
  virtual void resize(uint32_t, uint32_t) = 0;
  virtual void visibility(bool) = 0;
};
