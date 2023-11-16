# Raytracer-Project
RayTracer Diorama Inspired by Minecraft

Este proyecto es una implementación en C++ de un raytracer que renderiza un diorama inspirado en Minecraft usando la biblioteca SDL2 para gráficos. El raytracer simula la interacción de la luz con varios objetos de la escena, produciendo reflejos y efectos de iluminación realistas.

Asegúrese de tener instaladas las siguientes dependencias:
- SDL2
- SDL2_imagen

# How to Build

Clona el repositorio y construye el proyecto usando un compilador de C++. Asegúrese de vincular las bibliotecas SDL2 y SDL2_image.
```bash
g++ -o RayTracerDiorama main.cpp -lSDL2 -lSDL2_image
```

# Usage
Ejecute el ejecutable compilado para iniciar el trazador de rayos y visualizar el diorama inspirado en Minecraft.
```bash
./RayTracerDiorama
```

# Controls
- Flecha hacia arriba: mueve la cámara hacia arriba
- Flecha hacia abajo: mueve la cámara hacia abajo
- Flecha izquierda: gira la cámara hacia la izquierda
- Flecha derecha: gira la cámara hacia la derecha

# Scene Description

La escena se compone de varios objetos inspirados en Minecraft, incluido un Creeper, una cama, una mesa de manualidades y más. Cada objeto se define con sus propias propiedades materiales, como color, reflectividad y transparencia.



# Rendering Technique

El raytracer utiliza una técnica de trazado de rayos recursivo para simular el comportamiento de la luz en la escena. Calcula reflejos, refracciones y sombras para producir una representación realista. La escena también incluye un palco para un fondo visualmente atractivo.

# Representación Visual del Juego
![GIf](RayTracerProjectGif.gif)

