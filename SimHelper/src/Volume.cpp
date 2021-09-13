#include "../include/Volume.h"

namespace Helper{
    Volume::Volume(): bbox(Cuboid()){}

    Volume::Volume(const Cuboid& cuboid): bbox(cuboid){}

    Cuboid Volume::getBBox() const
    {
        return bbox;
    }
}