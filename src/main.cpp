#include <SDL2/SDL.h>
#include <SDL_blendmode.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <iostream>
#include <sstream>
#include "color.h"
#include "imageloader.h"
#include "raycaster.h"

SDL_Window *window;
SDL_Renderer *renderer;

bool menu = true;
int level = 1;

void clear()
{
  SDL_SetRenderDrawColor(renderer, 56, 56, 56, 255);
  SDL_RenderClear(renderer);
}

void draw_floor()
{
  // floor color
  SDL_SetRenderDrawColor(renderer, 112, 122, 122, 255);
  SDL_Rect rect = {
      0,
      (SCREEN_HEIGHT - MARGIN) / 2,
      SCREEN_WIDTH,
      (SCREEN_HEIGHT - MARGIN) / 2};
  SDL_RenderFillRect(renderer, &rect);
}

void draw_ui()
{
  std::string imageName;
  imageName = (menu ? "home" : "bg") + std::to_string(level);
  ImageLoader::render(renderer, imageName, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

Uint32 frameStart, frameTime;

int main(int argc, char *argv[])
{
  SDL_Init(SDL_INIT_VIDEO);
  ImageLoader::init();
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  window = SDL_CreateWindow("DOOM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  ImageLoader::loadImage("+", "../assets/w3.png", 64, 64);
  ImageLoader::loadImage("-", "../assets/w1.png", 64, 64);
  ImageLoader::loadImage("|", "../assets/w2.png", 64, 64);
  ImageLoader::loadImage("/", "../assets/w4.png", 64, 64);
  ImageLoader::loadImage("*", "../assets/w5.png", 64, 64);
  ImageLoader::loadImage("g", "../assets/w6.png", 64, 64);
  ImageLoader::loadImage("i", "../assets/w7.png", 64, 64);
  /* ImageLoader::loadImage("p", "assets/player.png"); */
  ImageLoader::loadImage("bg1", "../assets/fondo1.png", 440, 315);
  ImageLoader::loadImage("bg2", "../assets/fondo2.png", 440, 315);
  ImageLoader::loadImage("bg3", "../assets/fondo3.png", 440, 315);
  ImageLoader::loadImage("home1", "../assets/home1.png", 440, 315);
  ImageLoader::loadImage("home2", "../assets/home2.png", 440, 315);
  ImageLoader::loadImage("home3", "../assets/home3.png", 440, 315);
  ImageLoader::loadImage("e1", "../assets/sprite1.png", 128, 128);

  Raycaster r = {renderer};
  r.load_map("../assets/map.txt");

  bool running = true;
  while (running)
  {

    if (r.winner)
    {
      SDL_Log("you win");
      exit(1);
    }

    frameStart = SDL_GetTicks();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        running = false;
        break;
      }
      if (event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym)
        {
        case SDLK_LEFT:
          if (!menu)
          {
            r.player.a += 3.14 / 24;
          }
          break;
        case SDLK_RIGHT:
          if (!menu)
          {

            r.player.a -= 3.14 / 24;
          }
          break;
        case SDLK_UP:
          if (menu)
          {
            if (level > 1)
              level -= 1;
          }
          else
          {
            r.moveForward();
          }
          break;
        case SDLK_DOWN:
          if (menu)
          {
            if (level < 3)
              level += 1;
          }
          else
          {
            r.moveBack();
          }
          break;
        case SDLK_SPACE:
          if (menu)
            menu = false;
          break;
        default:
          break;
        }
      }

      if (event.type == SDL_MOUSEMOTION)
      {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= SCREEN_WIDTH - SCREEN_WIDTH / 3)
        {
          if (!menu)
          {
            r.player.a -= 3.14 / 24;
            if (r.player.a > 2 * M_PI)
              r.player.a -= 2 * M_PI;
          }
          break;
        }
        if (mouseX <= SCREEN_WIDTH / 3)
        {
          if (!menu)
          {
            r.player.a += 3.14 / 24;
            if (r.player.a < 2 * M_PI)
              r.player.a -= 2 * M_PI;
          }
          break;
        }
      }
    }

    clear();

    if (!menu)
    {
      draw_floor();

      r.render();
      r.renderMap();
    }

    draw_ui();
    // render

    SDL_RenderPresent(renderer);

    frameTime = SDL_GetTicks() - frameStart;

    if (frameTime > 0)
    {
      std::ostringstream titleStream;
      titleStream << "DOOM - " << 1000.0 / frameTime << " FPS"; // Milliseconds to seconds
      SDL_SetWindowTitle(window, titleStream.str().c_str());
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
