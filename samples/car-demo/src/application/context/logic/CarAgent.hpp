

#pragma once

#include "Line.hpp"

#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <array>

class Circuit;

class CarAgent {
public: // external structures
  struct t_sensor {
    Line line;
    float value;
  };
  typedef std::array<t_sensor, 5> t_sensors;

private: // attributes
  glm::vec2 _position;
  float _angle;
  float _speed;
  float _fitness;
  bool _alive;
  float _life = 1.0f;
  uint32_t _total_updates;

  // TODO : use enumeration
  t_sensors _sensors;

  uint32_t _current_checkpoint;

  std::vector<Line> _trail;

public: // ctor/dtor
  CarAgent();

public: // methods
  void update(float step, const Circuit& circuit, const NeuralNetwork& nn);
  void reset(const Circuit& circuit);

private: // methods
  void updateSensors();
  void collideSensors(const Lines& walls);
  void collideCheckpoints(const Lines& checkpoints);
  void collideWalls(const Lines& walls);

public: // setter/getter
  inline const glm::vec2&
  getPosition() const {
    return _position;
  }
  inline float
  getAngle() const {
    return _angle;
  }

  inline const t_sensors&
  getSensors() const {
    return _sensors;
  }

  inline float
  getFitness() const {
    return _fitness;
  }
  inline bool
  isAlive() const {
    return _alive;
  }

  float getLife() const;

  inline unsigned int
  getTotalUpdates() const {
    return _total_updates;
  }
  inline const std::vector<Line>&
  getTrail() const {
    return _trail;
  };
};
