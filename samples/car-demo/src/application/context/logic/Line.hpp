
#pragma once

#include "geronimo/helpers/GLMath.hpp"

#include <vector>

struct Line {
  glm::vec2 posA;
  glm::vec2 posB;
};
using Lines = std::vector<Line>;
