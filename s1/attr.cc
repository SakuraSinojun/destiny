
#include "attr.h"
#include "log.h"
#include "dice.h"

Attr::Attr()
    : str_(0)
    , dex_(0)
    , con_(0)
    , wis_(0)
    , int_(0)
    , chr_(0)
{
}

Attr::Attr(int i)
    : str_(i)
    , dex_(i)
    , con_(i)
    , wis_(i)
    , int_(i)
    , chr_(i)
{
    if (i == -1) {
        str_ = dice(6, 3);
        dex_ = dice(6, 3);
        con_ = dice(6, 3);
        wis_ = dice(6, 3);
        int_ = dice(6, 3);
        chr_ = dice(6, 3);
    }
}


Attr::~Attr()
{
}

int Attr::GetAttr(std::string attr)
{
    if (attr == "str")
        return Str();
    if (attr == "dex")
        return Dex();
    if (attr == "con")
        return Con();
    if (attr == "wis")
        return Wis();
    if (attr == "int")
        return Int();
    if (attr == "chr")
        return Chr();
    LOG(FATAL) << "attr == " << attr;
    return 0;
}

