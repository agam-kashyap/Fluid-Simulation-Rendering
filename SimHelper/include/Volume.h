#ifndef VOLUME_DEFINE
#define VOLUME_DEFINE

#include "point.h"

namespace Helper
{
    struct Cuboid
    {
        Point3D startingPoint;
        double width, length, height; // x, y, z axis

        Cuboid() :
            startingPoint(Point3D()),
            width(0.),
            length(0.),
            height(0.) {}

        Cuboid(const Point3D& _startingPoint, const double _width, const double _length, const double _height) :
            startingPoint(_startingPoint),
            width(_width),
            length(_length),
            height(_height) {}
    };

    class Volume
    {
        public:
            Volume();
            explicit Volume(const Cuboid& cuboid);
            ~Volume()=default;
            Cuboid getBBox() const;
        private:
            Cuboid bbox;
    };
}

#endif