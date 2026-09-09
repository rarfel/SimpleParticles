#include "headers/window.h"
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ctime>

struct SDLWindowState;

int InitSDL()
{
  if(!SDL_Init(SDL_INIT_VIDEO))
    {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error when initializing SDL3", nullptr);
      return 1;
    }
  return 0;
}

int CreateWindowAndRenderer(const char *title, SDL_WindowFlags flags, SDLWindowState &state)
{
  if (!SDL_CreateWindowAndRenderer(title, state.width, state.height, flags, &state.window, &state.renderer))
    {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
      return 1;
    }
  return 0;
}

void LoopHandler(SDLWindowState state, glm::vec4 backgroundColor, Particle particles[], size_t sizeParticlues)
{
  bool running = true;
  uint64_t previousTime = SDL_GetTicks();
  const float PI = 3.1415;
  float speed = 0;
  srand(time(NULL));

  while(running)
  {
    uint64_t nowTime = SDL_GetTicks();
    float deltaTime = (nowTime - previousTime) / 1000.0;
    SDL_Event event { 0 };

    while (SDL_PollEvent(&event))
    {
      running = EventHandler(&state, event);
    }
    DrawBackground(state, backgroundColor);

    MoveParticles(state, particles, sizeParticlues, speed, deltaTime);

    //swap buffers and show to screen
    SDL_RenderPresent(state.renderer);

    previousTime = nowTime;
  }
  CleanUp(state);
}

void Attract(Particle particles[], size_t sizeParticlues, float vel, float deltaTime, int i)
{
  for (int j = i; j < sizeParticlues; j++)
  {
    particles[i].speedX = vel;
    particles[i].speedY = vel;

    particles[i].particle.x += ((particles[j].particle.x - particles[i].particle.x) * particles[i].speedX) * deltaTime;
    particles[i].particle.y += ((particles[j].particle.y - particles[i].particle.y) * particles[i].speedY) * deltaTime;
  }
}

void MoveParticles(SDLWindowState state, Particle particles[], size_t sizeParticlues, float speed, float deltaTime)
{
  float radius = 10;
  for (int i = 0; i < sizeParticlues; i++)
  {
    InitParticles(state, particles[i]);
    if (particles[i].active)
    {
      SDL_SetRenderDrawColor(state.renderer, particles[i].color.r, particles[i].color.g, particles[i].color.b, 255);
      SDL_RenderFillRect(state.renderer, &particles[i].particle);

      CheckCollision(state,particles[i],speed);
      Attract(particles, sizeParticlues, 0.1, deltaTime, i);
    }
  }
}

void InitParticles(SDLWindowState state, Particle &particles)
{
  int size = 10;
  if (!particles.active)
  {
    particles.active = true;
    particles.collided = false;
    particles.particle.w = size;
    particles.particle.h = size;

    particles.particle.x = rand() % (int)state.width;
    particles.particle.y = rand() % (int)state.height;
    particles.color = {255, 0, 0};
  }
}

void CheckCollision(SDLWindowState state, Particle &particles, float speed)
{
  int percentage = 2; // roll a random number between 0 and 1 [0,1]
  if(particles.particle.x > state.width)
  {
    particles.particle.x = 0; 
    particles.speedX = (rand() % percentage) >= 1 ? speed: -(speed);
  }
  if(particles.particle.x < 0)
  {
    particles.particle.x = state.width;
    particles.speedX = (rand() % percentage) >= 1 ? speed: -(speed);
  }

  if(particles.particle.y > state.height)
  {
    particles.particle.y = 0;
    particles.speedY = (rand() % percentage) >= 1 ? speed: -(speed);
  }
  if(particles.particle.y < 0)
  {
    particles.particle.y = state.height;
    particles.speedY = (rand() % percentage) >= 1 ? speed: -(speed);
  }
}

bool EventHandler(SDLWindowState *state, SDL_Event &event)
{
  switch (event.type)
  {
    case SDL_EVENT_QUIT:
    {
      return false;
      break;
    }
    case SDL_EVENT_WINDOW_RESIZED:
    {
      state->width = event.window.data1;
      state->height = event.window.data2;
      break;
    }
  }
  return true;
}

void DrawBackground(SDLWindowState state, glm::vec4 backgroundColor)
{
  SDL_SetRenderDrawColorFloat(state.renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
  SDL_RenderClear(state.renderer);
}

void CleanUp(SDLWindowState &state)
{
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();
}
