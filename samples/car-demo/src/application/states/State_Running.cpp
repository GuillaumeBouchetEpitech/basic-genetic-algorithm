
#include "State_Running.hpp"

#include "StateManager.hpp"

#include "geronimo/graphics/input-managers/KeyboardManager.hpp"
// #include "geronimo/graphics/input-managers/MouseManager.hpp"
// #include "geronimo/graphics/input-managers/TouchManager.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/Scene.hpp"
#include "application/defines.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/physics/queries/ray-caster/RayCaster.hpp"
#include "geronimo/system/easing/easingFunctions.hpp"
#include "geronimo/system/math/angles.hpp"
#include "geronimo/system/math/clamp.hpp"
#include "geronimo/system/math/constants.hpp"
#include "geronimo/system/math/lerp.hpp"
#include "geronimo/system/math/safe-normalize.hpp"

void
State_Running::enter() {
  D_MYLOG("step");

  // SDL_SetRelativeMouseMode(SDL_TRUE);
  // MouseManager::get().setLock(true);
  // MouseManager::get().reset();
}

void
State_Running::leave() {
  D_MYLOG("step");

  // SDL_SetRelativeMouseMode(SDL_FALSE);
  // MouseManager::get().setLock(false);
}

void
State_Running::handleEvent(const SDL_Event& event) {
  // auto& context = Context::get();

  auto& keyboard = KeyboardManager::get();
  // auto& mouse = MouseManager::get();

  switch (event.type) {
  case SDL_KEYDOWN: {
    keyboard.updateAsPressed(event.key.keysym.sym);
    break;
  }
  case SDL_KEYUP: {
    keyboard.updateAsReleased(event.key.keysym.sym);
    break;
  }

    //   case SDL_MOUSEBUTTONDOWN: {
    //     // SDL_BUTTON_LEFT
    //     // SDL_BUTTON_MIDDLE
    //     // SDL_BUTTON_RIGHT

    //     mouse.updateAsPressed(event.button.button);
    //     break;
    //   }
    //   case SDL_MOUSEBUTTONUP: {
    //     mouse.updateAsReleased(event.button.button);
    //     break;
    //   }
    //   case SDL_MOUSEMOTION: {

    //     float deltaX = float(event.motion.xrel);
    //     float deltaY = float(event.motion.yrel);

    // #ifdef D_WEB_BUILD
    //     deltaX *= 0.5f;
    //     deltaY *= 0.5f;
    // #else
    // #endif

    //     mouse.updateDelta(deltaX, deltaY);

    //     break;
    //   }

  default:
    break;
  }
}

void
State_Running::update(uint32_t delta) {
  static_cast<void>(delta); // unused

  // float elapsedTime = float(delta) / 1000.0f;

  auto& context = Context::get();

  auto& perfProfiler = context.logic.performanceProfiler;

  perfProfiler.stop("FRAME");
  perfProfiler.start("FRAME");

  perfProfiler.start("1 update");

  //

  auto& keyboard = KeyboardManager::get();
  uint32_t totalSteps = 3;
  float lerpRatio = 0.05f;

  const bool accelerate = keyboard.isPressed(SDLK_UP);
  const bool decelerate = keyboard.isPressed(SDLK_DOWN);

  auto& cameraScale = context.graphic.scene.cameraScale;
  if (accelerate) {
    totalSteps = 100;
    cameraScale = gero::math::lerp(cameraScale, 3.0f, 0.05f);
    lerpRatio = 0.0f;
  } else if (decelerate) {
    totalSteps = 1;
    cameraScale = gero::math::lerp(cameraScale, 0.9f, 0.05f);
  } else {
    totalSteps = 5;
    cameraScale = gero::math::lerp(cameraScale, 1.0f, 0.05f);
  }

  for (uint32_t kk = 0; kk < totalSteps; ++kk) {
    constexpr float step = 0.25f;

    bool someone_is_alive = false;

    auto& allCarAgents = context.logic.allCarAgents;
    auto& allNeuralNetworks = context.logic.allNeuralNetworks;

    std::size_t bestIndex = 0;
    float bestFitness = 0.0f;

    // #pragma omp parallel num_threads(3)
    {
      // This code will be executed by three threads.

      // Chunks of this loop will be divided amongst
      // the (three) threads of the current team.
      // #pragma omp for
      for (std::size_t ii = 0; ii < allCarAgents.size(); ++ii) {
        if (!allCarAgents[ii].isAlive()) {
          continue;
        }

        someone_is_alive = true;

        allCarAgents[ii].update(step, context.logic.circuit, allNeuralNetworks[ii]);

        if (allCarAgents[ii].getFitness() > bestFitness + 5.0f) {
          bestFitness = allCarAgents[ii].getFitness();
          bestIndex = ii;
        }
      }
    }

    context.graphic.scene.leaderIndex = -1;
    if (someone_is_alive) {

      const auto& targetPos = allCarAgents.at(bestIndex).getPosition();
      context.graphic.scene.leaderIndex = int32_t(bestIndex);

      auto& cameraCenter = context.graphic.scene.cameraCenter;
      cameraCenter = gero::math::lerp(cameraCenter, targetPos, lerpRatio);

    } else {

      // rate genomes
      for (std::size_t ii = 0; ii < allCarAgents.size(); ++ii) {
        context.logic.geneticAlgorithm.rateGenome(ii, allCarAgents[ii].getFitness());
      }

      float generationBestFitness = -1.0f;
      std::size_t generationBestIndex = 0;
      for (std::size_t ii = 0; ii < allCarAgents.size(); ++ii) {
        if (allCarAgents[ii].getFitness() > generationBestFitness) {
          generationBestFitness = allCarAgents[ii].getFitness();
          generationBestIndex = ii;
        }
      }

      if (generationBestFitness > context.logic.previousLeaderFitness) {
        context.logic.previousLeaderFitness = generationBestFitness;

        const CarAgent& leaderAgent = context.logic.allCarAgents.at(generationBestIndex);

        context.logic.trails.push_back(leaderAgent.getTrail());
        while (context.logic.trails.size() > 5) {
          context.logic.trails.erase(context.logic.trails.begin());
        }
      }

      context.logic.geneticAlgorithm.breedPopulation();

      for (std::size_t ii = 0; ii < context.logic.geneticAlgorithm.getTotalGenomes(); ++ii) {
        const AbstractGenome& currGenome = context.logic.geneticAlgorithm.getGenome(ii);

        context.logic.allNeuralNetworks.at(ii).setConnectionsWeights(currGenome.getConnectionsWeights());
      }

      //
      //

      for (CarAgent& car : context.logic.allCarAgents) {
        car.reset(context.logic.circuit);
      }
    }
  }

  // MouseManager::get().resetDelta();

  perfProfiler.stop("1 update");
}

void
State_Running::render(const SDL_Window&) {
  // static_cast<void>(window); // <= unused

  auto& context = Context::get();

  auto& perfProfiler = context.logic.performanceProfiler;
  perfProfiler.start("2 render");

  const auto& cameraCenter = context.graphic.scene.cameraCenter;

  const auto& cameraScale = context.graphic.scene.cameraScale;
  context.graphic.camera.scene.setOrthographic(
    -400.0f * cameraScale, 400.0f * cameraScale, -300.0f * cameraScale, 300.0f * cameraScale, 100.0f, 600.0f);

  // context.logic.controllers.freeFly.syncCamera(context.graphic.camera.scene);
  context.graphic.camera.scene.lookAt(
    glm::vec3(cameraCenter.x, cameraCenter.y, 500), glm::vec3(cameraCenter.x, cameraCenter.y, 0), glm::vec3(0, -1, 0));

  Scene::renderAll();

  perfProfiler.stop("2 render");
}

void
State_Running::resize(uint32_t width, uint32_t height) {
  auto& context = Context::get();
  auto& graphic = context.graphic;

  graphic.camera.viewportSize = {width, height};
  graphic.camera.scene.setSize(width, height);
  graphic.camera.hud.setSize(width, height);
}

void
State_Running::visibility(bool visible) {
  auto* stateManager = StateManager::get();
  StateManager::States currentState = stateManager->getState();

  if (currentState != StateManager::States::Paused && !visible)
    stateManager->changeState(StateManager::States::Paused);
}
