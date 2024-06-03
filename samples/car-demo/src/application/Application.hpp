
#pragma once

#include "geronimo/graphics/sdl/SDLWindowWrapper.hpp"

#include <cstdint>

class Application : public gero::graphics::SDLWindowWrapper {
public:
  struct Definition {
    uint32_t width;
    uint32_t height;
  };

public:
  Application(const Definition& def);
  virtual ~Application();

public:
  void setMouseLockStatus(bool isLocked);

protected:
  virtual void _onEvent(const SDL_Event& event) override;
  virtual void _onUpdate(uint32_t deltaTime) override;
  virtual void _onRender(const SDL_Window& screen) override;
  virtual void _onResize(uint32_t width, uint32_t height) override;
  virtual void _onVisibilityChange(bool visible) override;
};
