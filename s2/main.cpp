
#include "ui.h"
#include "tools/logging.h"

#include <fstream>
#include <sstream>

int main()
{
    std::ofstream   f;
    f.open("log.txt");
    logging::setLoggingStream(f);

    ui  u;
    return u.run();
}

