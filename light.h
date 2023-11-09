#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include "color.h"
#include <vector>
#include <SDL2/SDL.h>

struct Light {
  glm::vec3 position;
  float intensity;
  Color color;

  Light(const glm::vec3& pos, float intensity, const Color& col) : position(pos), intensity(intensity), color(col) {}
};
