
#include "point.h"

std::ostream& operator<<(std::ostream& o, const point& pt)
{
    o << "(" << pt.x << ", " << pt.y << ")";
    return o;
}

