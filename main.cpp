#include <SDL.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SPIN_SIZE = 32;
const int GRID_WIDTH = SCREEN_WIDTH / SPIN_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / SPIN_SIZE;

bool init(SDL_Window *&window, SDL_Renderer *&renderer);
void close(SDL_Window *&window, SDL_Renderer *&renderer);
void renderSpins(SDL_Renderer *&renderer, int grid[GRID_WIDTH][GRID_HEIGHT]);

int main(int argc, char *argv[]) {
  srand(time(NULL));

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  if (!init(window, renderer)) {
    std::cerr << "Failed to initialize SDL. 😳" << std::endl;
    return 1;
  }

  int grid[GRID_WIDTH][GRID_HEIGHT];
  for (int i = 0; i < GRID_WIDTH; ++i) {
    for (int j = 0; j < GRID_HEIGHT; ++j) {
      grid[i][j] = rand() % 2 ? 1 : -1;
    }
  }

  bool quit = false;
  SDL_Event e;

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    renderSpins(renderer, grid);
    SDL_RenderPresent(renderer);
  }

  close(window, renderer);

  return 0;
}

bool init(SDL_Window *&window, SDL_Renderer *&renderer) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError()
              << std::endl;
    return false;
  }

  window = SDL_CreateWindow("2D Ising Spin System", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cerr << "Window could not be created! SDL Error: " << SDL_GetError()
              << std::endl;
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError()
              << std::endl;
    return false;
  }

  return true;
}

void close(SDL_Window *&window, SDL_Renderer *&renderer) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void renderSpins(SDL_Renderer *&renderer, int grid[GRID_WIDTH][GRID_HEIGHT]) {
  for (int i = 0; i < GRID_WIDTH; ++i) {
    for (int j = 0; j < GRID_HEIGHT; ++j) {
      int spin = grid[i][j];
      SDL_SetRenderDrawColor(renderer, spin > 0 ? 0 : 255, 0,
                             spin > 0 ? 255 : 0, 255);
      int x = i * SPIN_SIZE + SPIN_SIZE / 2;
      int y = j * SPIN_SIZE + SPIN_SIZE / 2;
      int r = SPIN_SIZE / 2;
      for (int dx = -r; dx <= r; ++dx) {
        for (int dy = -r; dy <= r; ++dy) {
          if (dx * dx + dy * dy <= r * r) {
            SDL_RenderDrawPoint(renderer, x + dx, y + dy);
          }
        }
      }
    }
  }
}
