
#pragma once

#include "IState.hpp"

class State_Running : public IState {
public:
  virtual void enter() override;
  virtual void leave() override;

public:
  virtual void handleEvent(const SDL_Event&) override;
  virtual void update(uint32_t deltaTime) override;
  virtual void render(const SDL_Window&) override;
  virtual void resize(uint32_t width, uint32_t height) override;
  virtual void visibility(bool visible) override;
};
