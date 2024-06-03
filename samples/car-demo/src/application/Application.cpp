
#include "Application.hpp"

#include "geronimo/graphics/input-managers/KeyboardManager.hpp"
#include "geronimo/graphics/input-managers/MouseManager.hpp"
#include "geronimo/graphics/input-managers/TouchManager.hpp"

//
#include "states/StateManager.hpp"

#include "context/Context.hpp"
#include "context/graphics/Scene.hpp"

#include "geronimo/system/TraceLogger.hpp"

#include <chrono>

namespace {

#if defined D_NATIVE_BUILD
constexpr bool k_canResize = true;
constexpr uint32_t k_frameRate = 60;
#else
constexpr bool k_canResize = false;
constexpr uint32_t k_frameRate = 0;
#endif

} // namespace

Application::Application(const Definition& def)
  : SDLWindowWrapper("Car Demo", def.width, def.height, k_frameRate, SDLWindowWrapper::OpenGlEsVersion::v3, k_canResize) {

  KeyboardManager::create();
  MouseManager::create();
  Context::create(def.width, def.height);
  StateManager::create();

  Scene::initialize();
}

Application::~Application() {
  StateManager::destroy();
  Context::destroy();
  MouseManager::destroy();
  KeyboardManager::destroy();
}

//

void
Application::setMouseLockStatus(bool isLocked) {

  auto& mouse = MouseManager::get();
  if (mouse.setLock(isLocked))
    mouse.reset();
}

//

void
Application::_onEvent(const SDL_Event& event) {
  StateManager::get()->handleEvent(event);
}

void
Application::_onUpdate(uint32_t deltaTime) {

  constexpr uint32_t k_maxDelta = 16U;
  if (deltaTime > k_maxDelta)
    deltaTime = k_maxDelta; // cap it

  StateManager::get()->update(deltaTime);
}

void
Application::_onRender(const SDL_Window& screen) {
  StateManager::get()->render(screen);
}

void
Application::_onResize(uint32_t width, uint32_t height) {
  StateManager::get()->resize(width, height);
}

void
Application::_onVisibilityChange(bool visible) {
  StateManager::get()->visibility(visible);
}
