#include <SDL.h>
#include <SDL_haptic.h>
#include <SDL_keycode.h>
#include <cstdlib>
#include <ctime>
#include <cwchar>
#include <functional>
#include <ios>
#include <iostream>
#include <map>
#include <string>

constexpr int SCREEN_WIDTH = 1600;
constexpr int SCREEN_HEIGHT = 1000;
constexpr int SPIN_SIZE = 20;
constexpr int GRID_WIDTH = SCREEN_WIDTH / SPIN_SIZE;
constexpr int GRID_HEIGHT = SCREEN_HEIGHT / SPIN_SIZE;

using Grid = int[GRID_WIDTH][GRID_HEIGHT];

bool init(SDL_Window *&window, SDL_Renderer *&renderer);
void close(SDL_Window *&window, SDL_Renderer *&renderer);
void renderSpins(SDL_Renderer *&renderer, Grid &grid);
void flip_any_spin(Grid &grid);
void flip_a_spin(Grid &grid, double H, double T, int attempts);
int count_spins(Grid &grid);

int main(int argc, char *argv[]) {
  srand(time(NULL));

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  if (!init(window, renderer)) {
    std::cerr << "Failed to initialize SDL. 😳" << std::endl;
    return 1;
  }

  Grid grid;
  auto init_grid = [&grid]() {
    for (int i = 0; i < GRID_WIDTH; ++i) {
      for (int j = 0; j < GRID_HEIGHT; ++j) {
        grid[i][j] = rand() % 2 ? 1 : -1;
      }
    }
  };

  init_grid();

  bool quit = false;
  SDL_Event e{};
  bool simulation_running = true;
  double H = 0.0;
  double T = 0.0001;

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }

      if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        int i = x / SPIN_SIZE;
        int j = y / SPIN_SIZE;
        grid[i][j] *= -1;

        simulation_running = false;
      }

      // start/stop simulation with space
      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_SPACE:
          simulation_running = !simulation_running;
          break;
        case SDLK_ESCAPE:
        case SDLK_q:
          quit = true;
          break;
        case SDLK_r:
          init_grid();
          break;
        }
      }
    }

    if (simulation_running) {
      for (int a = 0; a < 10; a++) {
        for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i) {
          flip_a_spin(grid, H, T, 1);
        }
        H = -count_spins(grid) / (double)(GRID_WIDTH * GRID_HEIGHT);
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

using neighbor_spin_fn = std::function<int(Grid &, int, int)>;

enum class BoundaryCondition {
  Dirichlet,
  Dirichlet_Positive,
  Dirichlet_Negative,
  Neumann,
  Periodic,
};

const std::map<BoundaryCondition, neighbor_spin_fn> neighbor_functions{
    {BoundaryCondition::Dirichlet,
     [](Grid &grid, int i, int j) {
       int neighbors = 0;
       if (i < GRID_WIDTH - 1) {
         neighbors += grid[i + 1][j];
       }
       if (i > 0) {
         neighbors += grid[i - 1][j];
       }
       if (j < GRID_HEIGHT - 1) {
         neighbors += grid[i][j + 1];
       }
       if (j > 0) {
         neighbors += grid[i][j - 1];
       }
       return neighbors;
     }},
    {BoundaryCondition::Dirichlet_Positive,
     [](Grid &grid, int i, int j) {
       int neighbors = 0;
       if (i < GRID_WIDTH - 1) {
         neighbors += grid[i + 1][j];
       } else {
         neighbors += 1;
       }
       if (i > 0) {
         neighbors += grid[i - 1][j];
       } else {
         neighbors += 1;
       }
       if (j < GRID_HEIGHT - 1) {
         neighbors += grid[i][j + 1];
       } else {
         neighbors += 1;
       }
       if (j > 0) {
         neighbors += grid[i][j - 1];
       } else {
         neighbors += 1;
       }
       return neighbors;
     }},
    {BoundaryCondition::Dirichlet_Negative,
     [](Grid &grid, int i, int j) {
       int neighbors = 0;
       if (i < GRID_WIDTH - 1) {
         neighbors += grid[i + 1][j];
       } else {
         neighbors -= 1;
       }
       if (i > 0) {
         neighbors += grid[i - 1][j];
       } else {
         neighbors -= 1;
       }
       if (j < GRID_HEIGHT - 1) {
         neighbors += grid[i][j + 1];
       } else {
         neighbors -= 1;
       }
       if (j > 0) {
         neighbors += grid[i][j - 1];
       } else {
         neighbors -= 1;
       }
       return neighbors;
     }},
    {BoundaryCondition::Neumann,
     [](Grid &grid, int i, int j) {
       int neighbors = 0;
       int own_spin = grid[i][j];
       if (i < GRID_WIDTH - 1) {
         neighbors += grid[i + 1][j];
       } else {
         neighbors += own_spin;
       }
       if (i > 0) {
         neighbors += grid[i - 1][j];
       } else {
         neighbors += own_spin;
       }

       if (j < GRID_HEIGHT - 1) {
         neighbors += grid[i][j + 1];
       } else {
         neighbors += own_spin;
       }
       if (j > 0) {
         neighbors += grid[i][j - 1];
       } else {
         neighbors += own_spin;
       }
       return neighbors;
     }},
    {BoundaryCondition::Periodic,
     [](Grid &grid, int i, int j) {
       int neighbors = 0;
       if (i < GRID_WIDTH - 1) {
         neighbors += grid[i + 1][j];
       } else {
         neighbors += grid[0][j];
       }
       if (i > 0) {
         neighbors += grid[i - 1][j];
       } else {
         neighbors += grid[GRID_WIDTH - 1][j];
       }

       if (j < GRID_HEIGHT - 1) {
         neighbors += grid[i][j + 1];
       } else {
         neighbors += grid[i][0];
       }
       if (j > 0) {
         neighbors += grid[i][j - 1];
       } else {
         neighbors += grid[i][GRID_HEIGHT - 1];
       }
       return neighbors;
     }},
};

double flip_rate(Grid &grid, int i, int j, double H, double T,
                 neighbor_spin_fn neighbor_spins) {
  int spin = grid[i][j];
  int neighbors = neighbor_spins(grid, i, j);

  double J = 1.0;
  double E = -spin * (H + J * neighbors);

  return 1.0 / (1.0 + exp(-2.0 * E / T));
}

int count_spins(Grid &grid) {
  int count = 0;
  for (int i = 0; i < GRID_WIDTH; ++i) {
    for (int j = 0; j < GRID_HEIGHT; ++j) {
      count += grid[i][j];
    }
  }
  return count;
}

double highest_flip_rate(double H, double T) {
  int neighbors = 0;
  int spin = 0;

  double E = -spin * (H + neighbors);
  return 1.0 / (1.0 + exp(-2.0 * E / T));
}

void flip_any_spin(Grid &grid) {
  int i = rand() % GRID_WIDTH;
  int j = rand() % GRID_HEIGHT;

  grid[i][j] *= -1;
}

void flip_a_spin(Grid &grid, double H, double T, int attempts) {
  for (int attempt = 0; attempt < attempts; attempt++) {
    int i = rand() % GRID_WIDTH;
    int j = rand() % GRID_HEIGHT;

    double r = highest_flip_rate(H, T) * rand() / (double)RAND_MAX;
    if (r < flip_rate(grid, i, j, H, T,
                      neighbor_functions.at(BoundaryCondition::Periodic))) {
      grid[i][j] *= -1;
      break;
    }
  }
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

void renderSpins(SDL_Renderer *&renderer, Grid &grid) {
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
