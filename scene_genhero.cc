

#include "scene_genhero.h"
#include <iostream>
#include "ui.h"
#include "log.h"

bool SceneGenHero::run(void)
{
    std::string n;
    UI::uninit();
    LInfo() << "input your name.";
    std::cin >> n;
    UI::init();
    return true;
}
 
