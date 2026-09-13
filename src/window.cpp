#include "headers/window.h"
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
  bool restart = false;
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
      running = EventHandler(&state, event, restart);
    }
    DrawBackground(state, backgroundColor);

    MoveParticles(state, particles, sizeParticlues, speed, deltaTime, restart);

    //swap buffers and show to screen
    SDL_RenderPresent(state.renderer);

    previousTime = nowTime;
  }
  CleanUp(state);
}

void Repel(Particle particles[], size_t sizeParticlues, float deltaTime, int i, int j)
{
  float xAxis;
  float yAxis;
  float force = 0.1;

  xAxis = particles[j].particle.x - particles[i].particle.x;
  yAxis = particles[j].particle.y - particles[i].particle.y;

  // this repels from first to last - 1
  particles[i].particle.x += ((-xAxis) * particles[i].speedX * force) * deltaTime;
  particles[i].particle.y += ((-yAxis) * particles[i].speedY * force) * deltaTime;

  // this repels the last
  if(j >= sizeParticlues - 1)
  {
    particles[j].particle.x += ((xAxis) * particles[j].speedX * force) * deltaTime;
    particles[j].particle.y += ((yAxis) * particles[j].speedY * force) * deltaTime;
  }

  particles[0].color = {0,255,0}; // marking the first particle
  particles[sizeParticlues-1].color = {0,0,255}; // marking the last particle
}


void MoveParticles(SDLWindowState state, Particle particles[], size_t sizeParticlues, float speed, float deltaTime, bool &restart)
{
  for (int i = 0; i < sizeParticlues; i++)
  {
    InitParticles(state, particles[i]);
    if (particles[i].active)
    {
      SDL_SetRenderDrawColor(state.renderer, particles[i].color.r, particles[i].color.g, particles[i].color.b, 255);
      SDL_RenderFillRect(state.renderer, &particles[i].particle);

      for (int j = i + 1; j < sizeParticlues; j++)
      {
        if(particles[j].active)
        {
          Attract(particles, sizeParticlues, 0.1, deltaTime, i, j);
          Repel(particles, sizeParticlues, deltaTime, i, j);
        }
      }
      WallCollision(state, particles, i);
    }
    if (restart)
    {
      RestartPartilcles(state,particles[i]);
    }
  }

  restart = false;
}

void WallCollision(SDLWindowState state, Particle particles[], int i)
{
  if(particles[i].particle.x > state.width)
  {particles[i].particle.x = -particles[i].particle.w;}

  if(particles[i].particle.x < -particles[i].particle.w)
  {particles[i].particle.x = state.width;}

  if(particles[i].particle.y > state.height)
  {particles[i].particle.y = -particles[i].particle.h;}

  if(particles[i].particle.y < -particles[i].particle.h)
  {particles[i].particle.y = state.height;}
}

void Attract(Particle particles[], size_t sizeParticlues, float vel, float deltaTime, int i, int j)
{
  float xAxis;
  float yAxis;

  particles[i].speedX = vel;
  particles[i].speedY = vel;

  particles[j].speedX = vel;
  particles[j].speedY = vel;

  xAxis = particles[j].particle.x - particles[i].particle.x;
  yAxis = particles[j].particle.y - particles[i].particle.y;

  // this attracts from first to last - 1
  particles[i].particle.x += ((xAxis) * particles[i].speedX) * deltaTime;
  particles[i].particle.y += ((yAxis) * particles[i].speedY) * deltaTime;

  // this attracts the last
  if(j >= sizeParticlues - 1)
  {
    particles[j].particle.x += ((-xAxis) * particles[j].speedX) * deltaTime;
    particles[j].particle.y += ((-yAxis) * particles[j].speedY) * deltaTime;
  }

  particles[0].color = {0,255,0}; // marking the first particle
  particles[sizeParticlues-1].color = {0,0,255}; // marking the last particle
}

void InitParticles(SDLWindowState state, Particle &particles)
{
  int size = 20;
  if (!particles.active)
  {
    particles.active = true;
    particles.particle.w = size;
    particles.particle.h = size;

    particles.particle.x = rand() % (int)state.width;
    particles.particle.y = rand() % (int)state.height;
    particles.color = {255, 0, 0};
  }
}

void RestartPartilcles(SDLWindowState state, Particle &particles)
{
  if (particles.active)
  {
    particles.active = false;

    particles.particle.x = rand() % (int)(state.width * 0.75);
    particles.particle.y = rand() % (int)(state.height * 1.1);
    particles.color = {255, 0, 0};
  }
}

bool EventHandler(SDLWindowState *state, SDL_Event &event, bool &restart)
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
    case SDL_EVENT_KEY_UP:
      if (event.key.key == SDLK_R)
      {
        SDL_Log("Restarting...");
        restart = true;
      }
      break;
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
