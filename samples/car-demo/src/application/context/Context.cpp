
#include "Context.hpp"

#include "application/defines.hpp"
// #include "logic/weapon-systems/weapons/WeaponBuilder.hpp"

#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/graphics/make-geometries/MakeGeometries.hpp"
#include "geronimo/physics/body/AbstractPhysicBody.hpp"
#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

#include <iomanip>
#include <sstream>

//
//
// singleton

Context* Context::_instance = nullptr;

Context::~Context() {}

void
Context::initialize(unsigned int width, unsigned int height) {
  graphic.camera.viewportSize = {width, height};

  //
  //
  //

  D_MYLOG("MaxTextureSize: " << gero::graphics::GlContext::getMaxTextureSize());

  {
    const auto& vSize = graphic.camera.viewportSize;

    // graphic.camera.scene.setPerspective(70.0f, 200.0f, 600.0f);
    graphic.camera.scene.setOrthographic(-400.0f, 400.0f, -300.0f, 300.0f, 100.0f, 600.0f);

    graphic.camera.hud.setOrthographic(0.0f, float(vSize.x), 0.0f, float(vSize.y), -10.0f, +10.0f);

    const glm::vec3 eye = {0.0f, 0.0f, 1.0f};
    const glm::vec3 center = {0.0f, 0.0f, 0.0f};
    const glm::vec3 upAxis = {0.0f, 1.0f, 0.0f};
    graphic.camera.hud.lookAt(eye, center, upAxis);
  }

  //
  //
  //

  graphic.stackRenderers.initialize("../../thirdparties/dependencies/geronimo/src");

  graphic.hud.textRenderer.initialize("../../thirdparties/dependencies/geronimo/src");

  {
    constexpr uint32_t k_totalGenomes = 300;

    logic.circuit.loadMap("./assets/Map_test.txt");

    gero::rng::RNG::ensureRandomSeed();

    GeneticAlgorithm::Definition genAlgoDef;
    genAlgoDef.topology.init({5, 4, 3, 2}, /*useBiasNeuron =*/true);
    genAlgoDef.totalGenomes = k_totalGenomes;
    genAlgoDef.minimumMutations = 2;
    genAlgoDef.mutationMaxChance = 0.2f;
    genAlgoDef.mutationMaxEffect = 0.2f;
    genAlgoDef.getRandomCallback = []() { return gero::rng::RNG::getNormalisedValue(); };

    logic.geneticAlgorithm.initialize(genAlgoDef);

    logic.allNeuralNetworks.reserve(k_totalGenomes);
    for (uint32_t ii = 0; ii < k_totalGenomes; ++ii) {
      const AbstractGenome& currGenome = logic.geneticAlgorithm.getGenome(ii);

      logic.allNeuralNetworks.emplace_back(genAlgoDef.topology);
      logic.allNeuralNetworks.back().setConnectionsWeights(currGenome.getConnectionsWeights());
    }

    logic.allCarAgents.resize(k_totalGenomes);
    for (auto& currCar : logic.allCarAgents) {
      currCar.reset(logic.circuit);
    }
  }
}

//

void
Context::create(unsigned int width, unsigned int height) {
  if (_instance)
    D_THROW(std::runtime_error, "Context singleton already initialized");

  _instance = new Context();
  _instance->initialize(width, height);
}

void
Context::destroy() {
  if (!_instance)
    D_THROW(std::runtime_error, "Context singleton already destroyed");

  delete _instance, _instance = nullptr;
}

Context&
Context::get() {
  if (!_instance)
    D_THROW(std::runtime_error, "Context singleton not initialized");

  return *_instance;
}

// singleton
//
//
