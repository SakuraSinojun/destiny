

#include "log.h"
#include "log_dispatcher.h"
#include "logtracer_shell.h"
#include "logtracer_udp.h"

#include "ui.h"
#include "game.h"
#include "dice.h"

#include <locale.h>

int main()
{
    setlocale(LC_ALL, "");
    // logging::LogDispatcher::GetInstance()->AddTracer(new logging::LogTracerShell());
    logging::LogDispatcher::GetInstance()->AddTracer(new logging::LogTracerUdp());
    dice_init();
    UI::init();
    Game *  g = Game::Get();
    g->Start();
    UI::uninit();
    return 0;
}

