
#pragma once

class Scene {
public:
  static void initialize();
  static void renderAll();

private:
  static void _clear();
  static void _updateMatrices();

  static void _renderScene();
  static void _renderHud();
};
