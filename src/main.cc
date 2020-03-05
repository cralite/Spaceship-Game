#include <cstdio>
#include <cstdlib>

#include <SDL.h>
#include "game.h"


int main(int argc, char **argv)
{
  Game game{};
  game.gameLoop();

  
  return EXIT_SUCCESS;
}