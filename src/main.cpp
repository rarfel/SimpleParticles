#include "headers/window.h"
#include "SDL3/SDL_main.h"

int main(int argc, char *argv[])
{
  SDLWindowState state;
  state.width = 1200;
  state.height = 900;

  InitSDL();

  CreateWindowAndRenderer("Particles", SDL_WINDOW_RESIZABLE, state);

  glm::vec4 backgroundColor = {0.0, 0.0, 0.0, 1.0};

  Particle particles[1000];
  size_t sizeParticles = sizeof(particles)/sizeof(particles[0]);

  LoopHandler(state, backgroundColor, particles, sizeParticles);
}
