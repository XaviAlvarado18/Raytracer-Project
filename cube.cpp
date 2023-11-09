#include "cube.h"
#include "SDL2/SDL.h"

Cube::Cube(const glm::vec3& minCorner, const glm::vec3& maxCorner, const Material& mat, SDL_Texture* texture)
    : minCorner(minCorner), maxCorner(maxCorner), Object(mat), texture(texture) {}


Intersect Cube::rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const {
    glm::vec3 tMin = (minCorner - rayOrigin) / rayDirection;
    glm::vec3 tMax = (maxCorner - rayOrigin) / rayDirection;

    glm::vec3 t1 = glm::min(tMin, tMax);
    glm::vec3 t2 = glm::max(tMin, tMax);

    float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
    float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

    if (tNear > tFar || tFar < 0) {
        return Intersect{false};
    }

    glm::vec3 point = rayOrigin + tNear * rayDirection;

    glm::vec3 normal;
    if (point.x < minCorner.x + 1e-5) {
        normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    } else if (point.x > maxCorner.x - 1e-5) {
        normal = glm::vec3(1.0f, 0.0f, 0.0f);
    } else if (point.y < minCorner.y + 1e-5) {
        normal = glm::vec3(0.0f, -1.0f, 0.0f);
    } else if (point.y > maxCorner.y - 1e-5) {
        normal = glm::vec3(0.0f, 1.0f, 0.0f);
    } else if (point.z < minCorner.z + 1e-5) {
        normal = glm::vec3(0.0f, 0.0f, -1.0f);
    } else if (point.z > maxCorner.z - 1e-5) {
        normal = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    return Intersect{true, tNear, point, normal};
}

void Cube::render(SDL_Renderer* renderer, SDL_Texture* texture) {
    int w, h;
    
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    SDL_Rect quad[6];
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            quad[i].x = corners[(i + j) % 4].x - w / 2;
            quad[i].y = corners[(i + j) % 4].y - h / 2;
            quad[i].w = w;
            quad[i].h = h;
        }
    }

    SDL_RenderCopyEx(renderer, texture, NULL, quad, 0.0f, NULL, SDL_FLIP_NONE);
}