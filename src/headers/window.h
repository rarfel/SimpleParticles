#pragma once
#include "SDL3/SDL.h"
#include "glm/glm.hpp"

struct SDLWindowState
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  float width = -1;
  float height = -1;
};

struct Particle
{
  SDL_FRect particle = {-1,-1,-1,-1};
  float speedX = 0;
  float speedY = 0;
  bool active = false;
  glm::ivec3 color = {0,0,0};
};

int InitSDL();

int CreateWindowAndRenderer(const char *title, SDL_WindowFlags flags, SDLWindowState &state);
bool EventHandler(SDLWindowState *state, SDL_Event &event, bool &restart);
void DrawBackground(SDLWindowState state, glm::vec4 backgroundColor);
void DrawBackground(SDLWindowState state, glm::vec4 backgroundColor);
void CleanUp(SDLWindowState &state);

void LoopHandler(SDLWindowState state, glm::vec4 backgroundColor, Particle particles[], size_t sizeParticlues);

void InitParticles(SDLWindowState state, Particle &particles);
void RestartPartilcles(SDLWindowState state, Particle &particles);
void WallCollision(SDLWindowState state, Particle particles[], int i);

void Attract(Particle particles[], size_t sizeParticlues, float vel, float deltaTime, int i, int j);

void MoveParticles(SDLWindowState state, Particle particles[], size_t sizeParticlues, float speed, float deltaTime, bool &restart);
