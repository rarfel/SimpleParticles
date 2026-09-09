#pragma once
#include "SDL3/SDL.h"
#include "glm/glm.hpp"

struct SDLWindowState
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  float width;
  float height;
};

struct Particle
{
  SDL_FRect particle;
  float speedX;
  float speedY;
  bool active = false;
  bool collided;
  glm::ivec3 color;
};

int InitSDL();

int CreateWindowAndRenderer(const char *title, SDL_WindowFlags flags, SDLWindowState &state);

void LoopHandler(SDLWindowState state, glm::vec4 backgroundColor, Particle particles[], size_t sizeParticlues);

bool EventHandler(SDLWindowState *state, SDL_Event &event);

void CheckCollision(SDLWindowState state, Particle &particles, float speed);

void InitParticles(SDLWindowState state, Particle &particles);

void MoveParticles(SDLWindowState state, Particle particles[], size_t sizeParticlues, float speed, float deltaTime);

void DrawBackground(SDLWindowState state, glm::vec4 backgroundColor);

void DrawBackground(SDLWindowState state, glm::vec4 backgroundColor);

void CleanUp(SDLWindowState &state);
