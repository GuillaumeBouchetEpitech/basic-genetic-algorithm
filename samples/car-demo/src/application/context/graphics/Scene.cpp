
#include "Scene.hpp"

#include "application/context/Context.hpp"

#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/advanced-concept/widgets/helpers/renderTextBackground.hpp"
#include "geronimo/graphics/advanced-concept/widgets/helpers/writeTime.hpp"
#include "geronimo/graphics/advanced-concept/widgets/renderHistoricalTimeData.hpp"
#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/getStreamStr.hpp"
#include "geronimo/system/math/angles.hpp"
#include "geronimo/system/math/safe-normalize.hpp"
#include "geronimo/system/math/rotateVec2.hpp"
#include "geronimo/system/math/clamp.hpp"
#include "geronimo/system/math/lerp.hpp"
#include "geronimo/system/math/constants.hpp"
#include "geronimo/system/math/circleCircleIntersectionPoints.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"
#include "geronimo/system/string-utils/stream-formatter.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <iomanip>

using namespace gero::graphics;
using namespace gero::graphics::GlContext;

void
Scene::initialize() {
  GlContext::enable(GlContext::States::depthTest);
  GlContext::setDepthFunc(GlContext::DepthFuncs::less);

  GlContext::disable(GlContext::States::blend);

  GlContext::enable(GlContext::States::cullFace);
}

void
Scene::renderAll() {
  Scene::_clear();
  Scene::_updateMatrices();

  auto& context = Context::get();
  auto& performanceProfiler = context.logic.performanceProfiler;

  performanceProfiler.start("2 render-scene");

  Scene::_renderScene();

  performanceProfiler.stop("2 render-scene");

  performanceProfiler.start("2 render-hud");

  Scene::_renderHud();

  performanceProfiler.stop("2 render-hud");
}

void
Scene::_renderScene() {

  GlContext::enable(GlContext::States::cullFace);

  auto& context = Context::get();
  // auto& logic = context.logic;
  auto& graphic = context.graphic;
  auto& camera = graphic.camera;
  gero::graphics::Camera& camInstance = camera.scene;

  const auto& matricesData = camInstance.getMatricesData();

  graphic.stackRenderers.setMatricesData(matricesData);

  auto& wireFrames = graphic.stackRenderers.getWireFramesStack();
  auto& triangles = graphic.stackRenderers.getTrianglesStack();

  {

    {

      wireFrames.pushLine(glm::vec3(0, 0, 0), glm::vec3(3, 0, 0), glm::vec3(1.0f, 0.5f, 0.5f));
      wireFrames.pushLine(glm::vec3(0, 0, 0), glm::vec3(0, 3, 0), glm::vec3(0.5f, 1.0f, 0.5f));
      wireFrames.pushLine(glm::vec3(0, 0, 0), glm::vec3(0, 0, 3), glm::vec3(0.5f, 0.5f, 1.0f));

      graphic.stackRenderers.flush();
    }

    {

      {
        const glm::vec3 color = glm::vec3(0.5f, 0.5f, 1.0f);
        const auto& walls = context.logic.circuit.getWalls();
        for (const Line& currLine : walls)
        {
          wireFrames.pushLine(
            glm::vec3(currLine.posA, 0),
            glm::vec3(currLine.posB, 0),
            color);
        }
      }

      {
        const glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);
        const auto& checkpoints = context.logic.circuit.getCheckpoints();
        for (const Line& currLine : checkpoints)
        {
          wireFrames.pushLine(
            glm::vec3(currLine.posA, 0),
            glm::vec3(currLine.posB, 0),
            color);
        }
      }

      // auto _renderCar = [&context, &triangles, &wireFrames](const CarAgent &currCar, bool isLeader)
      // {

      //   const glm::vec3 pos3d = glm::vec3(currCar.getPosition(), 0);

      //   std::array<glm::vec3, 4> _vertices1 = {{
      //     gero::math::rotate2d(pos3d + glm::vec3(-20,-10,0), currCar.getPosition(), currCar.getAngle()),
      //     gero::math::rotate2d(pos3d + glm::vec3(+20,-10,0), currCar.getPosition(), currCar.getAngle()),
      //     gero::math::rotate2d(pos3d + glm::vec3(+20,+10,0), currCar.getPosition(), currCar.getAngle()),
      //     gero::math::rotate2d(pos3d + glm::vec3(-20,+10,0), currCar.getPosition(), currCar.getAngle()),
      //   }};
      //   std::array<glm::vec3, 4> _vertices2 = {{
      //     gero::math::rotate2d(pos3d + glm::vec3(-20+2,-10+2,1), currCar.getPosition(), currCar.getAngle()),
      //     gero::math::rotate2d(pos3d + glm::vec3(+20-2,-10+2,1), currCar.getPosition(), currCar.getAngle()),
      //     gero::math::rotate2d(pos3d + glm::vec3(+20-2,+10-2,1), currCar.getPosition(), currCar.getAngle()),
      //     gero::math::rotate2d(pos3d + glm::vec3(-20+2,+10-2,1), currCar.getPosition(), currCar.getAngle()),
      //   }};
      //   // const glm::vec3 negX_negY = gero::math::rotate2d(pos3d + glm::vec3(-20,-10,0), currCar.getPosition(), currCar.getAngle());
      //   // const glm::vec3 posX_negY = gero::math::rotate2d(pos3d + glm::vec3(+20,-10,0), currCar.getPosition(), currCar.getAngle());
      //   // const glm::vec3 posX_posY = gero::math::rotate2d(pos3d + glm::vec3(+20,+10,0), currCar.getPosition(), currCar.getAngle());
      //   // const glm::vec3 negX_posY = gero::math::rotate2d(pos3d + glm::vec3(-20,+10,0), currCar.getPosition(), currCar.getAngle());

      //   const glm::vec3 color = currCar.isAlive() ? glm::vec3(0.55f,0.55f,1.0f) : glm::vec3(0.5f,0.0f,0.0f);

      //   // wireFrames.pushLine(negX_negY, posX_negY, color);
      //   // wireFrames.pushLine(posX_negY, posX_posY, color);
      //   // wireFrames.pushLine(posX_posY, negX_posY, color);
      //   // wireFrames.pushLine(negX_posY, negX_negY, color);

      //   triangles.pushTriangle(_vertices1[0], _vertices1[2], _vertices1[1], glm::vec4(1,1,1, 1.0f));
      //   triangles.pushTriangle(_vertices1[0], _vertices1[3], _vertices1[2], glm::vec4(1,1,1, 1.0f));

      //   triangles.pushTriangle(_vertices2[0], _vertices2[2], _vertices2[1], glm::vec4(color, 1.0f));
      //   triangles.pushTriangle(_vertices2[0], _vertices2[3], _vertices2[2], glm::vec4(color, 1.0f));

      //   // triangles.pushThickTriangle3dLine(_vertices1[0], _vertices1[1], 2.0f, glm::vec4(0,0,0, 1.0f));
      //   // triangles.pushThickTriangle3dLine(_vertices1[1], _vertices1[2], 2.0f, glm::vec4(0,0,0, 1.0f));
      //   // triangles.pushThickTriangle3dLine(_vertices1[2], _vertices1[3], 2.0f, glm::vec4(0,0,0, 1.0f));
      //   // triangles.pushThickTriangle3dLine(_vertices1[3], _vertices1[0], 2.0f, glm::vec4(0,0,0, 1.0f));

      //   // const bool isLeader = context.graphic.scene.leaderIndex == int32_t(ii);
      //   if (!isLeader) {
      //     return;
      //   }

      //   auto colorEasing = gero::easing::GenericEasing<5, glm::vec3>()
      //     .push(0.00f, glm::vec3(1.0f,0.0f,0.0f))
      //     .push(0.25f, glm::vec3(1.0f,0.5f,0.2f))
      //     .push(0.50f, glm::vec3(1.0f,1.0f,0.0f))
      //     .push(0.75f, glm::vec3(0.0f,1.0f,0.0f))
      //     .push(1.00f, glm::vec3(0.0f,1.0f,0.0f))
      //     ;

      //   for (const auto& currSensor : currCar.getSensors()) {

      //     const glm::vec3 sensorColor = colorEasing.get(currSensor.value);

      //     const glm::vec2 dstPos = gero::math::lerp(currSensor.line.posA, currSensor.line.posB, currSensor.value);

      //     wireFrames.pushLine(glm::vec3(currSensor.line.posA, 0), glm::vec3(dstPos, 0), sensorColor);
      //   }

      // };

      for (std::size_t ii = 0; ii < context.logic.allCarAgents.size(); ++ii)
      {
        const auto& currCar = context.logic.allCarAgents[ii];

        // _renderCar(currCar, false);

        const bool isLeader = context.graphic.scene.leaderIndex == int32_t(ii);

        const glm::vec3 pos3d = glm::vec3(currCar.getPosition(), isLeader ? 2 : 0);

        std::array<glm::vec3, 4> _vertices1 = {{
          gero::math::rotate2d(pos3d + glm::vec3(-20,-10,0), currCar.getPosition(), currCar.getAngle()),
          gero::math::rotate2d(pos3d + glm::vec3(+20,-10,0), currCar.getPosition(), currCar.getAngle()),
          gero::math::rotate2d(pos3d + glm::vec3(+20,+10,0), currCar.getPosition(), currCar.getAngle()),
          gero::math::rotate2d(pos3d + glm::vec3(-20,+10,0), currCar.getPosition(), currCar.getAngle()),
        }};
        std::array<glm::vec3, 4> _vertices2 = {{
          gero::math::rotate2d(pos3d + glm::vec3(-20+2,-10+2,1), currCar.getPosition(), currCar.getAngle()),
          gero::math::rotate2d(pos3d + glm::vec3(+20-2,-10+2,1), currCar.getPosition(), currCar.getAngle()),
          gero::math::rotate2d(pos3d + glm::vec3(+20-2,+10-2,1), currCar.getPosition(), currCar.getAngle()),
          gero::math::rotate2d(pos3d + glm::vec3(-20+2,+10-2,1), currCar.getPosition(), currCar.getAngle()),
        }};
        // const glm::vec3 negX_negY = gero::math::rotate2d(pos3d + glm::vec3(-20,-10,0), currCar.getPosition(), currCar.getAngle());
        // const glm::vec3 posX_negY = gero::math::rotate2d(pos3d + glm::vec3(+20,-10,0), currCar.getPosition(), currCar.getAngle());
        // const glm::vec3 posX_posY = gero::math::rotate2d(pos3d + glm::vec3(+20,+10,0), currCar.getPosition(), currCar.getAngle());
        // const glm::vec3 negX_posY = gero::math::rotate2d(pos3d + glm::vec3(-20,+10,0), currCar.getPosition(), currCar.getAngle());

        const glm::vec3 color = currCar.isAlive()
          ? isLeader
            ? glm::vec3(0.0f,1.0f,0.0f)
            : glm::vec3(0.55f,0.55f,1.0f)
          : glm::vec3(0.5f,0.0f,0.0f);

        // wireFrames.pushLine(negX_negY, posX_negY, color);
        // wireFrames.pushLine(posX_negY, posX_posY, color);
        // wireFrames.pushLine(posX_posY, negX_posY, color);
        // wireFrames.pushLine(negX_posY, negX_negY, color);

        triangles.pushTriangle(_vertices1[0], _vertices1[2], _vertices1[1], glm::vec4(0.7f,0.7f,0.7f, 1.0f));
        triangles.pushTriangle(_vertices1[0], _vertices1[3], _vertices1[2], glm::vec4(0.7f,0.7f,0.7f, 1.0f));

        triangles.pushTriangle(_vertices2[0], _vertices2[2], _vertices2[1], glm::vec4(color, 1.0f));
        triangles.pushTriangle(_vertices2[0], _vertices2[3], _vertices2[2], glm::vec4(color, 1.0f));

        // triangles.pushThickTriangle3dLine(_vertices1[0], _vertices1[1], 2.0f, glm::vec4(0,0,0, 1.0f));
        // triangles.pushThickTriangle3dLine(_vertices1[1], _vertices1[2], 2.0f, glm::vec4(0,0,0, 1.0f));
        // triangles.pushThickTriangle3dLine(_vertices1[2], _vertices1[3], 2.0f, glm::vec4(0,0,0, 1.0f));
        // triangles.pushThickTriangle3dLine(_vertices1[3], _vertices1[0], 2.0f, glm::vec4(0,0,0, 1.0f));

        if (!isLeader) {
          continue;
        }

        auto colorEasing = gero::easing::GenericEasing<5, glm::vec3>()
          .push(0.00f, glm::vec3(1.0f,0.0f,0.0f))
          .push(0.25f, glm::vec3(1.0f,0.5f,0.2f))
          .push(0.50f, glm::vec3(1.0f,1.0f,0.0f))
          .push(0.75f, glm::vec3(0.0f,1.0f,0.0f))
          .push(1.00f, glm::vec3(0.0f,1.0f,0.0f))
          ;

        for (const auto& currSensor : currCar.getSensors()) {

          const glm::vec3 sensorColor = colorEasing.get(currSensor.value);

          const glm::vec2 dstPos = gero::math::lerp(currSensor.line.posA, currSensor.line.posB, currSensor.value);

          wireFrames.pushLine(glm::vec3(currSensor.line.posA, 0), glm::vec3(dstPos, 0), sensorColor);
        }

      }


      // if (context.graphic.scene.leaderIndex >= 0)
      // {
      //   const auto& currCar = context.logic.allCarAgents[context.graphic.scene.leaderIndex];

      //   _renderCar(currCar, true);
      // }

    }

    graphic.stackRenderers.flush();
  }


  {

    for (const auto& trail : context.logic.trails)
    {
      for (const auto& line : trail)
      {
        wireFrames.pushLine(
          glm::vec3(line.posA.x, line.posA.y, 0.0f),
          glm::vec3(line.posB.x, line.posB.y, 0.0f),
          glm::vec3(0.35f, 0.35f, 0.35f)
        );
      }
    }

    graphic.stackRenderers.flush();
  }



}

void
Scene::_renderHud() {
  GlContext::clears(Buffers::depth);
  GlContext::disable(GlContext::States::depthTest);
  GlContext::disable(GlContext::States::cullFace);

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& camera = graphic.camera;
  gero::graphics::Camera& camInstance = camera.hud;

  const auto& matricesData = camInstance.getMatricesData();

  graphic.stackRenderers.setMatricesData(matricesData);

  {

    const glm::vec4 k_white = glm::vec4(1, 1, 1, 1);
    const glm::vec4 k_black = glm::vec4(0, 0, 0, 1);

    auto& textRenderer = graphic.hud.textRenderer;

    textRenderer.setMainColor(k_white).setOutlineColor(k_black).setScale(50);
    textRenderer.setHorizontalTextAlign(TextRenderer::HorizontalTextAlign::center);
    textRenderer.setVerticalTextAlign(TextRenderer::VerticalTextAlign::center);

    // textRenderer.pushText(glm::vec2(400, 600 - 55), "test");

    textRenderer.setScale(14);
    textRenderer.setHorizontalTextAlign(TextRenderer::HorizontalTextAlign::left);

    {

      const auto& timeDataMap = context.logic.performanceProfiler.getHistoricalTimeDataMap();

      const gero::graphics::TextRenderer::State state0 = {glm::vec4(1.0f,1.0f,1.0f,1), glm::vec4(0,0,0,1)};
      const gero::graphics::TextRenderer::State state1 = {glm::vec4(1.0f,1.0f,0.5f,1), glm::vec4(0,0,0,1)};
      const gero::graphics::TextRenderer::State state2 = {glm::vec4(0.5f,1.0f,0.5f,1), glm::vec4(0,0,0,1)};
      const gero::graphics::TextRenderer::State state3 = {glm::vec4(1.0f,0.5f,0.5f,1), glm::vec4(0,0,0,1)};

      std::stringstream sstr;
      for (const auto& timeData : timeDataMap) {

        if (timeData.first[0] != '1')
          continue;

        const int32_t avgIndex = timeData.second.getAverageDuration() > 0 ? 1 : 0;
        const int32_t minIndex = timeData.second.getMinDuration() > 0 ? 2 : 0;
        const int32_t maxIndex = timeData.second.getMaxDuration() > 0 ? 3 : 0;

        sstr << "${0}" << std::left << std::setw(15) << timeData.first;
        sstr << " " << std::setw(2) << timeData.second.getLatestDuration();
        sstr << " ${" << avgIndex << "}" << std::setw(2) << timeData.second.getAverageDuration();
        sstr << " ${" << minIndex << "}" << std::setw(2) << timeData.second.getMinDuration();
        sstr << " ${" << maxIndex << "}" << std::setw(2) << timeData.second.getMaxDuration();
        sstr << std::endl;
      }

      sstr << std::endl;
      sstr << std::endl;

      for (const auto& timeData : timeDataMap) {

        if (timeData.first[0] != '2')
          continue;

        const int32_t avgIndex = timeData.second.getAverageDuration() > 0 ? 1 : 0;
        const int32_t minIndex = timeData.second.getMinDuration() > 0 ? 2 : 0;
        const int32_t maxIndex = timeData.second.getMaxDuration() > 0 ? 3 : 0;

        sstr << "${0}" << std::left << std::setw(15) << timeData.first;
        sstr << " " << std::setw(2) << timeData.second.getLatestDuration();
        sstr << " ${" << avgIndex << "}" << std::setw(2) << timeData.second.getAverageDuration();
        sstr << " ${" << minIndex << "}" << std::setw(2) << timeData.second.getMinDuration();
        sstr << " ${" << maxIndex << "}" << std::setw(2) << timeData.second.getMaxDuration();
        sstr << std::endl;
      }

      // const glm::vec2 vSize = glm::vec2(graphic.camera.viewportSize);

      const glm::vec2 textPos = glm::vec2(5, 5);
      // const glm::vec2 textPos = glm::vec2(5, vSize.y - 5);
      const std::string str = sstr.str();

      textRenderer.setHorizontalTextAlign(TextRenderer::HorizontalTextAlign::left);
      textRenderer.setVerticalTextAlign(TextRenderer::VerticalTextAlign::bottom);

      textRenderer.pushText(textPos, str,
        state0,
        state1,
        state2,
        state3
        );

    }

    textRenderer.setMatricesData(matricesData);
    textRenderer.render();
  }

  const auto& performanceProfiler = context.logic.performanceProfiler;
  if (auto timeDataRef = performanceProfiler.tryGetTimeData("FRAME")) {
    const auto& timeData = timeDataRef->get();

    const glm::vec2 vSize = glm::vec2(graphic.camera.viewportSize);

    const glm::vec2 k_size = glm::vec2(150, 50);
    const glm::vec3 k_pos = glm::vec3(vSize.x - k_size.x - 110, vSize.y - k_size.y - 20, 0);

    gero::graphics::widgets::renderHistoricalTimeData(
      k_pos, k_size, true, timeData, graphic.stackRenderers, graphic.hud.textRenderer);

    graphic.stackRenderers.flush();
    graphic.hud.textRenderer.render();
  }

  GlContext::enable(GlContext::States::depthTest);
}

void
Scene::_clear() {
  const auto& viewportSize = Context::get().graphic.camera.viewportSize;

  GlContext::setViewport(0, 0, uint32_t(viewportSize.x), uint32_t(viewportSize.y));

  GlContext::clearColor(0.15f, 0.15f, 0.15f, 1.0f);
  GlContext::clearDepth(1.0f);
  GlContext::clears(Buffers::color, Buffers::depth);
}

void
Scene::_updateMatrices() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& camera = graphic.camera;

  camera.scene.setSize(camera.viewportSize.x, camera.viewportSize.y);
  camera.scene.computeMatrices();
  camera.hud.setSize(camera.viewportSize.x, camera.viewportSize.y);
  camera.hud.computeMatrices();
}
