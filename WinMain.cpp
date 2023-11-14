#include "SDL2/SDL.h"
#include <windows.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include "include/SDL_image.h"
#include <cstdlib>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "print.h"
#include "color.h"
#include "intersect.h"
#include "object.h"
#include "sphere.h"
#include "light.h"
#include "camera.h"
#include "cube.h"
#include "skybox.h"

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
const int MAX_RECURSION = 3;
const float BIAS = 0.0001f;
Skybox skybox("./assets/wallpaper2.jpg");


SDL_Renderer* renderer;
std::vector<Object*> objects;
Color colorC(0, 24, 255, 255); 
Light light(glm::vec3(-1.0, 0, 10), 1.5f, Color(255, 255, 255));
// Declarar la posición de la luz como una variable global
glm::vec3 lightPosition = glm::vec3(-1.0, -2, 10);
Camera camera(glm::vec3(0.0, 2.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f);



void point(glm::vec2 position, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, position.x, position.y);
}

float castShadow(const glm::vec3& shadowOrigin, const glm::vec3& lightDir, Object* hitObject) {
    for (auto& obj : objects) {
        if (obj != hitObject) {
            Intersect shadowIntersect = obj->rayIntersect(shadowOrigin, lightDir);
            if (shadowIntersect.isIntersecting && shadowIntersect.dist > 0) {
                float shadowRatio = shadowIntersect.dist / glm::length(light.position - shadowOrigin);
                shadowRatio = glm::min(1.0f, shadowRatio);
                return 1.0f - shadowRatio;
            }
        }
    }
    return 1.0f;
}

// Cargar una imagen como textura
SDL_Texture* loadTexture(const std::string& path) {
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        // Manejar el error
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    return texture;
}

// Cargar la textura de imagen
SDL_Texture* texture = loadTexture("./assets/emerald.png");

Color castRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const short recursion = 0) {
    float zBuffer = 99999;
    Object* hitObject = nullptr;
    Intersect intersect;

    for (const auto& object : objects) {
        Intersect i = object->rayIntersect(rayOrigin, rayDirection);
        if (i.isIntersecting && i.dist < zBuffer) {
            zBuffer = i.dist;
            hitObject = object;
            intersect = i;
        }
    }

    if (!intersect.isIntersecting || recursion == MAX_RECURSION) {
        //return Color(173, 216, 230);
        return skybox.getColor(rayDirection);  // Sky color
    }


    glm::vec3 lightDir = glm::normalize(light.position - intersect.point);
    glm::vec3 viewDir = glm::normalize(rayOrigin - intersect.point);
    glm::vec3 reflectDir = glm::reflect(-lightDir, intersect.normal); 

    float shadowIntensity = castShadow(intersect.point, lightDir, hitObject);

    float diffuseLightIntensity = std::max(0.0f, glm::dot(intersect.normal, lightDir));
    float specReflection = glm::dot(viewDir, reflectDir);
    
    Material mat = hitObject->material;

    float specLightIntensity = std::pow(std::max(0.0f, glm::dot(viewDir, reflectDir)), mat.specularCoefficient);


    Color reflectedColor(0.0f, 0.0f, 0.0f);
    if (mat.reflectivity > 0) {
        glm::vec3 origin = intersect.point + intersect.normal * BIAS;
        reflectedColor = castRay(origin, reflectDir, recursion + 1); 
    }

    Color refractedColor(0.0f, 0.0f, 0.0f);
    if (mat.transparency > 0) {
        glm::vec3 origin = intersect.point - intersect.normal * BIAS;
        glm::vec3 refractDir = glm::refract(rayDirection, intersect.normal, mat.refractionIndex);
        refractedColor = castRay(origin, refractDir, recursion + 1); 
    }



    Color diffuseLight = mat.diffuse * light.intensity * diffuseLightIntensity * mat.albedo * shadowIntensity;
    Color specularLight = light.color * light.intensity * specLightIntensity * mat.specularAlbedo * shadowIntensity;
    Color color = (diffuseLight + specularLight) * (1.0f - mat.reflectivity - mat.transparency) + reflectedColor * mat.reflectivity + refractedColor * mat.transparency;
    return color;
}


void setUp() {
    

    Material ivory = {
        Color(100, 100, 80),
        0.5,
        0.5,
        50.0f,
        0.4f,
        0.0f
    };

    Material green = {
        Color(95, 149, 83),
        0.8f,
        0.5f,
        32.0f,
        0.2f,
        0.0f,
        0.0f
    };

    Material brown = {
            Color(108,94,83),   // diffuse
            0.9,
            0.1,
            10.0f,
            0.0f,
            0.0f
    };

    Material glass = {
        Color(255, 255, 255),
        0.0f,
        10.0f,
        1425.0f,
        0.2f,
        1.0f,
    };

    Material rubber = {
        Color(80, 0, 0),   // diffuse
        0.9,
        0.1,
        10.0f,
        0.0f,
        0.0f
    };

    Material grass = {
        Color(34, 139, 34),   // Green grass color
        0.8,
        0.2,
        10.0f,
        0.0f,
        0.0f
    };


    Material leaves = {
        Color(0, 255, 0),   // Green leaves color
        0.8,
        0.2,
        10.0f,
        0.0f,
        0.0f
    };


    Material minecraftChestMaterial = {
        Color(139, 69, 19),  // Marrón claro (RGB)
        0.8f,                // Coeficiente de reflexión difusa
        0.2f,                // Coeficiente de reflexión especular
        50.0f,               // Coeficiente especular
        0.1f,                // Coeficiente de reflectividad
        0.0f,                // Coeficiente de transparencia (no transparente)
        1.0f                 // Índice de refracción (no se refracta)
    };

    Material horno = {
        Color(143, 143, 144),  // Marrón claro (RGB)
        0.8f,                // Coeficiente de reflexión difusa
        0.2f,                // Coeficiente de reflexión especular
        50.0f,               // Coeficiente especular
        0.1f,                // Coeficiente de reflectividad
        0.0f,                // Coeficiente de transparencia (no transparente)
        1.0f                 // Índice de refracción (no se refracta)
    };

    Material mesa_crafteo= {
        Color(104, 80, 52),  // Marrón claro (RGB)
        0.8f,                // Coeficiente de reflexión difusa
        0.2f,                // Coeficiente de reflexión especular
        50.0f,               // Coeficiente especular
        0.1f,                // Coeficiente de reflectividad
        0.0f,                // Coeficiente de transparencia (no transparente)
        1.0f                 // Índice de refracción (no se refracta)
    };

    Material minecraftGold = {
        Color(199, 172, 53),   // Gold block color
        0.8,
        0.2,
        50.0f,
        0.8f,
        0.0f,
        1.0f  // You may adjust refraction index based on your specific needs
    };    

    Material minecraftLapisLazuli = {
        Color(16, 43, 105),   // Lapis Lazuli block color
        0.8,
        0.2,
        30.0f,
        0.5f,
        0.0f,
        1.0f  // You may adjust refraction index based on your specific needs
    };

    Material minecraftDiamond = {
        Color(159, 245, 228),   // Diamond block color
        0.8,
        0.2,
        80.0f,
        0.9f,
        0.0f,
        2.4f  // You may adjust refraction index based on your specific needs
    };

    Material minecraftRedstone = {
        Color(63, 5, 0),   // Redstone dust color
        0.8,
        0.2,
        20.0f,
        0.3f,
        0.0f,
        1.0f  // You may adjust refraction index based on your specific needs
    };
        
    // Cuerpo principal del Creeper
    objects.push_back(new Cube(glm::vec3(0.25f, 1.0f, 0.15f), glm::vec3(0.05f, 1.6f, 0.45f), green));

    // Cabeza del Creeper
    objects.push_back(new Cube(glm::vec3(0.31f, 1.6f, 0.1f), glm::vec3(0.0f, 1.9f, 0.5f), green));

    // Ojoss
    objects.push_back(new Cube(glm::vec3(0.41f, 1.78f, 0.16f), glm::vec3(0.0f, 1.84f, 0.24f), minecraftChestMaterial));
    objects.push_back(new Cube(glm::vec3(0.41f, 1.78f, 0.44f), glm::vec3(0.0f, 1.84f, 0.36f), minecraftChestMaterial));

    // Boca
    objects.push_back(new Cube(glm::vec3(0.41f, 1.73f, 0.22f), glm::vec3(0.0f, 1.75f, 0.34f), minecraftChestMaterial));
    objects.push_back(new Cube(glm::vec3(0.41f, 1.67f, 0.18f), glm::vec3(0.0f, 1.73f, 0.42f), minecraftChestMaterial));
    
    objects.push_back(new Cube(glm::vec3(0.41f, 1.64f, 0.18f), glm::vec3(0.0f, 1.67f, 0.23f), minecraftChestMaterial));
    objects.push_back(new Cube(glm::vec3(0.41f, 1.64f, 0.37f), glm::vec3(0.0f, 1.67f, 0.42f), minecraftChestMaterial));


    // Piernas
    objects.push_back(new Cube(glm::vec3(0.31f, 0.8f, 0.15f), glm::vec3(0.0f, 1.0f, 0.28f), green));
    objects.push_back(new Cube(glm::vec3(0.31f, 0.8f, 0.32f), glm::vec3(0.0f, 1.0f, 0.45f), green));

    

    objects.push_back(new Cube(glm::vec3(0.31f, 0.8f, 0.1f), glm::vec3(-0.12f, 0.5f, 0.50f), glass)); //BLOQUE CREEPER


    


    objects.push_back(new Cube(glm::vec3(-3.0f, 0.5f, -3.0f), glm::vec3(0.3f, 0.3f, 0.5f), grass)); //BASE
    objects.push_back(new Cube(glm::vec3(-1.42f, 0.8f, -1.12f), glm::vec3(-3.00f, 0.5f, 0.50f), glass)); //GRAMA POR ENCIMA
    
    objects.push_back(new Cube(glm::vec3(-1.42f, 0.8f, -3.00f), glm::vec3(-3.00f, 0.5f, -1.98f), glass)); //GRAMA CAMA
    objects.push_back(new Cube(glm::vec3(-1.42f, 0.8f, -2.42f), glm::vec3(-1.09f, 0.5f, -2.10f), horno)); //HORNO
    objects.push_back(new Cube(glm::vec3(-1.42f, 0.8f, -2.10f), glm::vec3(-1.09f, 0.5f, -2.74f), mesa_crafteo)); //MESA DE CRAFTEO
    objects.push_back(new Cube(glm::vec3(0.30f, 0.8f, -1.75f), glm::vec3(-0.16f, 0.5f, -1.34f), ivory)); //GRAMA FRONTAL
    objects.push_back(new Cube(glm::vec3(-2.60f, 0.8f, -1.75f), glm::vec3(-3.00f, 0.5f, -1.34f), brown)); //GRAMA BACK
    objects.push_back(new Cube(glm::vec3(-2.60f, 1.1f, -1.98f), glm::vec3(-3.00f, 0.8f, -2.29f), brown)); //GRAMA ENCIMA CAMA
    objects.push_back(new Cube(glm::vec3(-2.60f, 1.7f, -2.70f), glm::vec3(-3.00f, 0.8f, -3.0f), brown)); //TRONCO

    //Hojas
    objects.push_back(new Cube(glm::vec3(-2.60f, 2.0f, -2.70f), glm::vec3(-3.00f, 1.7f, -3.0f), leaves)); 
    objects.push_back(new Cube(glm::vec3(-2.60f, 1.7f, -3.00f), glm::vec3(-3.00f, 1.4f, -3.30f), leaves));
    objects.push_back(new Cube(glm::vec3(-3.00f, 1.7f, -3.00f), glm::vec3(-3.40f, 1.4f, -3.30f), leaves));
    objects.push_back(new Cube(glm::vec3(-2.20f, 1.7f, -3.00f), glm::vec3(-2.60f, 1.4f, -3.30f), leaves));
    objects.push_back(new Cube(glm::vec3(-2.20f, 1.7f, -2.70f), glm::vec3(-2.60f, 1.4f, -3.00f), leaves));
    objects.push_back(new Cube(glm::vec3(-2.20f, 1.7f, -2.40f), glm::vec3(-2.60f, 1.4f, -2.70f), leaves));
    objects.push_back(new Cube(glm::vec3(-2.20f, 1.7f, -2.70f), glm::vec3(-2.60f, 1.4f, -3.00f), leaves));
    objects.push_back(new Cube(glm::vec3(-2.60f, 1.7f, -2.70f), glm::vec3(-3.00f, 1.4f, -3.00f), leaves));
    objects.push_back(new Cube(glm::vec3(-3.00f, 1.7f, -2.70f), glm::vec3(-3.40f, 1.4f, -3.00f), leaves));
    objects.push_back(new Cube(glm::vec3(-3.00f, 1.7f, -2.40f), glm::vec3(-3.40f, 1.4f, -2.70f), leaves));
    objects.push_back(new Cube(glm::vec3(-2.60f, 1.7f, -2.40f), glm::vec3(-3.00f, 1.4f, -2.70f), leaves));

    //CAMA
    objects.push_back(new Cube(glm::vec3(-2.45f, 1.0f, -2.70f), glm::vec3(-2.60f, 0.8f, -3.0f), glass));
    objects.push_back(new Cube(glm::vec3(-2.45f, 1.0f, -2.70f), glm::vec3(-1.90f, 0.8f, -3.0f), rubber)); 


    // Cuerpo principal del Creeper
    objects.push_back(new Cube(glm::vec3(-1.75f, 1.6f, -1.98f), glm::vec3(-1.95f, 1.0f, -2.30f), green));

    // Cabeza del Creeper
    objects.push_back(new Cube(glm::vec3(-1.70f, 1.6f, -1.93f), glm::vec3(-2.00f, 1.9f, -2.35f), green));

    // Ojoss
    objects.push_back(new Cube(glm::vec3(-1.60f, 1.78f, -2.34f), glm::vec3(-2.00f, 1.84f, -2.26f), minecraftChestMaterial));
    objects.push_back(new Cube(glm::vec3(-1.60f, 1.78f, -2.06f), glm::vec3(-2.00f, 1.84f, -2.14f), minecraftChestMaterial));

    // Boca
    objects.push_back(new Cube(glm::vec3(-1.60f, 1.73f, -2.28f), glm::vec3(-2.0f, 1.75f, -2.16f), minecraftChestMaterial));
    objects.push_back(new Cube(glm::vec3(-1.60f, 1.67f, -2.32f), glm::vec3(-2.0f, 1.73f, -2.08f), minecraftChestMaterial));
    objects.push_back(new Cube(glm::vec3(-1.60f, 1.64f, -2.32f), glm::vec3(-2.0f, 1.67f, -2.27f), minecraftChestMaterial));
    objects.push_back(new Cube(glm::vec3(-1.60f, 1.64f, -2.13f), glm::vec3(-2.0f, 1.67f, -2.08f), minecraftChestMaterial));

    // Piernas
    objects.push_back(new Cube(glm::vec3(-1.70f, 0.8f, -2.30f), glm::vec3(-2.0f, 1.0f, -2.17f), green));
    objects.push_back(new Cube(glm::vec3(-1.70f, 0.8f, -2.13f), glm::vec3(-2.0f, 1.0f, -2.00f), green));


    objects.push_back(new Cube(glm::vec3(-1.42f, 0.8f, -0.75f), glm::vec3(-1.09f, 0.5f, -1.12f), minecraftGold)); //ORO
    objects.push_back(new Cube(glm::vec3(-1.72f, 0.8f, -1.12f), glm::vec3(-1.43f, 0.5f, -1.42f), minecraftLapisLazuli)); //LAPIZLAZULI
    objects.push_back(new Cube(glm::vec3(-1.72f, 1.1f, -0.82f), glm::vec3(-1.43f, 0.8f, -1.12f), minecraftDiamond)); //DIAMANTE
    objects.push_back(new Cube(glm::vec3(-3.00f, 1.1f, -0.32f), glm::vec3(-2.43f, 0.8f, 0.50f), minecraftRedstone)); //REDSTONE

    objects.push_back(new Cube(glm::vec3(-3.00f, 1.4f, 0.22f), glm::vec3(-2.73f, 1.1f, 0.50f), minecraftChestMaterial)); 
}

void render() {
    float fov = 3.1415/3;
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            /*
            float random_value = static_cast<float>(std::rand())/static_cast<float>(RAND_MAX);
            if (random_value < 0.0) {
                continue;
            }
            */


            float screenX = (2.0f * (x + 0.5f)) / SCREEN_WIDTH - 1.0f;
            float screenY = -(2.0f * (y + 0.5f)) / SCREEN_HEIGHT + 1.0f;
            screenX *= ASPECT_RATIO;
            screenX *= tan(fov/2.0f);
            screenY *= tan(fov/2.0f);


            glm::vec3 cameraDir = glm::normalize(camera.target - camera.position);

            glm::vec3 cameraX = glm::normalize(glm::cross(cameraDir, camera.up));
            glm::vec3 cameraY = glm::normalize(glm::cross(cameraX, cameraDir));
            glm::vec3 rayDirection = glm::normalize(
                cameraDir + cameraX * screenX + cameraY * screenY
            );
           
            Color pixelColor = castRay(camera.position, rayDirection);
            //Color pixelColor = castRay(glm::vec3(0,0,20), glm::normalize(glm::vec3(screenX, screenY, -1.0f))); 

            point(glm::vec2(x, y), pixelColor);
        }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Hello World - FPS: 0", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime = startTime;
    
    setUp();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        camera.move(-1.0f);
                        lightPosition.y -= 1.0f;
                        break;
                    case SDLK_DOWN:
                        camera.move(1.0f);
                        lightPosition.y += 1.0f;
                        break;
                    case SDLK_LEFT:
                        print("left");
                        camera.rotate(-1.0f, 0.0f);
                        lightPosition.x -= 1.0f;
                        break;
                    case SDLK_RIGHT:
                        print("right");
                        camera.rotate(1.0f, 0.0f);
                        lightPosition.x += 1.0f;
                        break;
                 }
            }


        }

        // Actualizar la posición de la luz
        light.position = lightPosition;

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render();

        // Present the renderer
        SDL_RenderPresent(renderer);

        frameCount++;

        // Calculate and display FPS
        if (SDL_GetTicks() - currentTime >= 1000) {
            currentTime = SDL_GetTicks();
            std::string title = "Hello World - FPS: " + std::to_string(frameCount);
            SDL_SetWindowTitle(window, title.c_str());
            frameCount = 0;
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

