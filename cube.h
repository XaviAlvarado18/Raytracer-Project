#pragma once

#include "glm/glm.hpp"
#include "object.h"
#include "material.h"
#include "intersect.h"

class Cube : public Object {
public:
    Cube(const glm::vec3& minCorner, const glm::vec3& maxCorner, const Material& mat, SDL_Texture* texture);
    SDL_Texture* getTexture() const { return texture; }
    void render(SDL_Renderer* renderer, SDL_Texture* texture);

    Intersect rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const override;

    glm::vec3 getMinCorner() const {
        return minCorner;
    }

    glm::vec3 getMaxCorner() const {
        return maxCorner;
    }

private:
    glm::vec3 minCorner;
    glm::vec3 maxCorner;
    SDL_Texture* texture;
    glm::vec2 corners[4];
};