
#pragma once

#include "./logic/Circuit.hpp"
#include "./logic/CarAgent.hpp"

#include "basic-genetic-algorithm/GeneticAlgorithm.hpp"
#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/audio/OpenALSoundManager.hpp"
#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/metrics/PerformanceProfiler.hpp"

#include "geronimo/graphics/advanced-concept/textRenderer/TextRenderer.hpp"
#include "geronimo/graphics/advanced-concept/stackRenderers/StackRenderers.hpp"

#include <array>
#include <cstdint>
#include <list>
#include <memory> // <= unique_ptr / make_unique
#include <string>
#include <unordered_map>

class Context : public gero::NonCopyable {

  //
  //
  // singleton

private:
  static Context* _instance;

  Context() = default;
  ~Context();

private:
  void initialize(unsigned int width, unsigned int height);
  void initialize_physic();

public:
  static void create(unsigned int width, unsigned int height);
  static void destroy();
  static Context& get();

  // singleton
  //
  //

private:

public:
  struct Graphic {
    struct CameraData {
      glm::uvec2 viewportSize = {800, 600};

      gero::graphics::Camera scene;
      gero::graphics::Camera hud;
    } camera;

    gero::graphics::StackRenderers stackRenderers;

    struct Hud {
      gero::graphics::TextRenderer textRenderer;
    } hud;

    struct Scene {

      glm::vec2 cameraCenter = {0,0};
      float cameraScale = 1.0f;
      int32_t leaderIndex = -1;

    } scene;

  } graphic;

  //

  struct Logic {
    gero::metrics::PerformanceProfiler performanceProfiler;

    Circuit circuit;
    GeneticAlgorithm geneticAlgorithm;
    std::vector<NeuralNetwork> allNeuralNetworks;
	  std::vector<CarAgent> allCarAgents;
    float previousLeaderFitness = -1.0f;

    std::list<std::vector<Line>> trails;

    bool pauseMode = false;
    bool fastForward = false;
  } logic;
};
