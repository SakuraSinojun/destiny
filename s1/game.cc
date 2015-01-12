
#include "game.h"
#include "scene.h"
#include "scene_genhero.h"
#include "scene_game.h"

#include <string>
#include <stdlib.h>
#include <iostream>

static Game* g_game = NULL;
Game::Game()
    : scene(NULL)
{
}

Game::~Game()
{
}

Game* Game::Get()
{
    if (!g_game)
        g_game = new Game();
    return g_game;
}

void Game::Start(void)
{
    // GenHero();
    if (scene)
        delete scene;
    scene = new SceneGame();
    while (scene->run()) {
    }
}

void Game::GenHero(void)
{
    if (scene)
        delete scene;
    scene = new SceneGenHero();
    scene->run();
}
