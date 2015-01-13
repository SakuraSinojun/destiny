
#pragma once

#include "options.h"
#include "map.h"

class game
{
public:
    game();
    virtual ~game();

    void load(const char * name);

    options& o() { return m_options; }
    map&     m() { return m_map; }

private:
    options m_options;
    map     m_map;
};

