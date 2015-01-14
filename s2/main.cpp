
#include "ui.h"
#include "tools/logging.h"

#include "overmap.h"

#include <fstream>
#include <sstream>

int main()
{
    // FATAL() << sizeof(overmap);

    std::ofstream   f;
    f.open("log.txt");
    logging::setLoggingStream(f);

    ui  u;
    return u.run();
}

