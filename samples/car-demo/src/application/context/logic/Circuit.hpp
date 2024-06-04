

#pragma once

#include "Line.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <string>

class Circuit {
public: // ctor/dtor
  Circuit();
  ~Circuit();

public: // methods
  bool loadMap(const std::string& filename);

public: // setter/getter
  inline bool
  isValid() const {
    return m_valid;
  }
  inline const Lines&
  getCheckpoints() const {
    return m_checkpoints;
  }
  inline const Lines&
  getWalls() const {
    return m_walls;
  }

  inline const glm::vec2&
  getStartingPosition() const {
    return m_start_position;
  }
  inline float
  getStartingAngle() const {
    return m_start_angle;
  }

  inline const glm::vec2&
  getStoppingPosition() const {
    return m_stop_position;
  }
  inline float
  getStoppingAngle() const {
    return m_stop_angle;
  }

private: // attributes
  Lines m_checkpoints;
  Lines m_walls;

  glm::vec2 m_start_position;
  float m_start_angle;

  glm::vec2 m_stop_position;
  float m_stop_angle;

  bool m_valid;
};
