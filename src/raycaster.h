#pragma once

#include <iostream>
#include <fstream>
#include <SDL_render.h>
#include <string>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include <unordered_map>
#include "color.h"
#include "imageloader.h"

const Color B = {0, 0, 0};
const Color W = {255, 255, 255};

bool menu = true;
int level = 1;
bool winner = false;

const int WIDTH = 16;
const int HEIGHT = 11;
const int BLOCK = 25;

const int MARGIN = 20;

const int SCREEN_WIDTH = WIDTH * BLOCK + 2 * MARGIN;
const int SCREEN_HEIGHT = HEIGHT * BLOCK + 2 * MARGIN;

const int MAP_BLOCK = 10;
const int MAP_WIDTH = WIDTH * MAP_BLOCK;
const int MAP_HEIGHT = HEIGHT * MAP_BLOCK;

const int MAP_X = SCREEN_WIDTH - MAP_WIDTH - MARGIN;
const int MAP_Y = SCREEN_HEIGHT - MAP_HEIGHT - MARGIN;

struct Player
{
  int x;
  int y;
  float a;
  float fov;
};

struct Impact
{
  float d;
  std::string mapHit; // + | -
  int tx;
};

struct Enemy
{
  int x;
  int y;
  std::string texture;
};

std::vector<Enemy> enemies;

class Raycaster
{
public:
  Raycaster(SDL_Renderer *renderer)
      : renderer(renderer)
  {

    resetPlayerPosition();

    scale = 50;
    tsize = 128;

    enemies = {Enemy{BLOCK * 5, BLOCK, "e1"}};
  }

  void load_map(const std::string &filename)
  {
    std::ifstream file(filename);
    std::string line;
    while (getline(file, line))
    {
      map.push_back(line);
    }
    file.close();
  }

  void point(int x, int y, Color c)
  {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawPoint(renderer, x, y);
  }

  void rect(int x, int y, const std::string &mapHit)
  {

    glm::vec2 tsize = ImageLoader::getImageSize(mapHit);
    for (int cx = x; cx < x + BLOCK; cx++)
    {
      for (int cy = y; cy < y + BLOCK; cy++)
      {
        int tx = ((cx - x) * tsize.x) / BLOCK;
        int ty = ((cy - y) * tsize.y) / BLOCK;

        Color c = ImageLoader::getPixelColor(mapHit, tx, ty);
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
        SDL_RenderDrawPoint(renderer, cx, cy);
      }
    }
  }

  void mapRect(int x, int y, Color c)
  {

    for (int cx = x; cx < x + MAP_BLOCK; cx++)
    {
      for (int cy = y; cy < y + MAP_BLOCK; cy++)
      {

        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
        SDL_RenderDrawPoint(renderer, cx, cy);
      }
    }
  }

  void draw_enemy(Enemy enemy)
  {
    float enemy_a = atan2(enemy.y - player.y, enemy.x - player.x);
    float enemy_d = sqrt(pow(player.x - enemy.x, 2) + pow(player.y - enemy.y, 2));
    int enemy_size = (SCREEN_HEIGHT / enemy_d) * scale;

    int enemy_x = (enemy_a - player.a) * (SCREEN_WIDTH / player.fov) + SCREEN_WIDTH / 2.0f - enemy_size / 2.0f;
    int enemy_y = (SCREEN_HEIGHT / 2.0f) - enemy_size / 2.0f;

    for (int x = enemy_x; x < enemy_x + enemy_size; x++)
    {
      for (int y = enemy_y; y < enemy_y + enemy_size; y++)
      {
        int tx = (x - enemy_x) * tsize / enemy_size;
        int ty = (y - enemy_y) * tsize / enemy_size;

        Color c = ImageLoader::getPixelColor(enemy.texture, tx, ty);
        if (c.r != 152 && c.g != 0 && c.b != 136)
        {
          SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
          SDL_RenderDrawPoint(renderer, x, y);
        }
      }
    }
  }

  Impact cast_ray(float a)
  {
    float d = 0;
    std::string mapHit;
    int tx;

    while (true)
    {
      int x = static_cast<int>(player.x + d * cos(a));
      int y = static_cast<int>(player.y + d * sin(a));

      int i = static_cast<int>(x / BLOCK);
      int j = static_cast<int>(y / BLOCK);

      if (map[j][i] != ' ')
      {
        mapHit = map[j][i];

        int hitx = x - i * BLOCK;
        int hity = y - j * BLOCK;
        int maxhit;

        if (hitx == 0 || hitx == BLOCK - 1)
        {
          maxhit = hity;
        }
        else
        {
          maxhit = hitx;
        }

        float tsize = ImageLoader::getImageSize(mapHit).x;

        tx = maxhit * tsize / BLOCK;

        break;
      }

      /* point(x, y, W); */

      d += 1;
    }
    return Impact{d, mapHit, tx};
  }

  void draw_stake(int x, float h, Impact i)
  {
    float start = (SCREEN_HEIGHT - 2 * MARGIN) / 2.0f - h / 2.0f;
    float end = start + h;
    float tsize = ImageLoader::getImageSize(i.mapHit).y;

    for (int y = start; y < end; y++)
    {
      int ty = (y - start) * tsize / h;

      if (ty < 0)
        ty = 0;

      Color c = ImageLoader::getPixelColor(i.mapHit, i.tx, ty);
      SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

      SDL_RenderDrawPoint(renderer, x, y);
    }
  }

  void render()
  {

    // draw left side of the screen
    /*
     for (int x = 0; x < SCREEN_WIDTH; x += BLOCK) {
       for (int y = 0; y < SCREEN_HEIGHT; y += BLOCK) {
         int i = static_cast<int>(x / BLOCK);
         int j = static_cast<int>(y / BLOCK);

         if (map[j][i] != ' ') {
           std::string mapHit;
           mapHit = map[j][i];
           Color c = Color(255, 0, 0);
           rect(x, y, mapHit);
         }
       }
     }

     for (int i = 1; i < SCREEN_WIDTH; i++) {
       float a = player.a + player.fov / 2 - player.fov * i / SCREEN_WIDTH;
       cast_ray(a);
     }
 */
    // draw right side of the screen

    for (int i = MARGIN; i < SCREEN_WIDTH - MARGIN; i++)
    {
      double a = player.a + player.fov / 2.0 - player.fov * i / SCREEN_WIDTH;
      Impact impact = cast_ray(a);
      float d = impact.d;

      if (d <= 0)
      {
        if (impact.mapHit == "g")
        {
          winner = true;
          break;
        }
        else
        {
          player.x = lastPosition.x;
          player.y = lastPosition.y;
        }
      }

      int x = i;
      const int rend = 5;
      float h = static_cast<float>((SCREEN_HEIGHT - 2 * MARGIN) / rend) / static_cast<float>(d * cos(a - player.a)) * static_cast<float>(scale);
      draw_stake(x, h, impact);
    }
    // Guardar última posición exitosa
    lastPosition = glm::vec2(player.x, player.y);
    /*
    for(Enemy enemy : enemies) {
      draw_enemy(enemy);
    }
    */
  }

  void renderMap()
  {
    for (int x = MAP_X; x < MAP_X + MAP_WIDTH; x += MAP_BLOCK)
    {
      for (int y = MAP_Y; y < MAP_Y + MAP_HEIGHT; y += MAP_BLOCK)
      {
        int i = static_cast<int>((x - MAP_X) / MAP_BLOCK);
        int j = static_cast<int>((y - MAP_Y) / MAP_BLOCK);

        if (map[j][i] != ' ')
        {
          std::string mapHit;
          mapHit = map[j][i];
          mapRect(x, y, Color(255, 255, 255));
        }
      }
    }

    // renderizar jugador
    int px = static_cast<int>((player.x / BLOCK) * MAP_BLOCK + MAP_X);
    int py = static_cast<int>((player.y / BLOCK) * MAP_BLOCK + MAP_Y);
    mapRect(px, py, Color(255, 0, 0));
  }

  void moveForward()
  {
    player.x += moveSpeed * cos(player.a);
    player.y += moveSpeed * sin(player.a);
  }

  void moveBack()
  {
    player.x -= moveSpeed * cos(player.a);
    player.y -= moveSpeed * sin(player.a);
  }

  void resetPlayerPosition()
  {
    player.x = BLOCK + BLOCK / 2;
    player.y = BLOCK + BLOCK / 2;

    player.a = M_PI / 4.0f;
    player.fov = M_PI / 3.0f;
  }

  Player player;
  glm::vec2 lastPosition;
  int moveSpeed = 10;

private:
  int scale;
  SDL_Renderer *renderer;
  std::vector<std::string> map;
  int tsize;
};
