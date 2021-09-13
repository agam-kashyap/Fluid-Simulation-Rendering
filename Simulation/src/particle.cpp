#include "../include/particle.h"

namespace SPH
{
    Particle::Particle() :
        radius(0.0),
        density(0.0),
        pressure(0.0),
        mass(0.0),
        // supportRadius(0.0),
        position(Helper::Point3D()),
        velocity(Helper::Point3D()),
        acceleration(Helper::Point3D()),
        fGravity(Helper::Point3D()),
        // fSurfaceTension(Helper::Point3D()),
        fViscosity(Helper::Point3D()),
        fPressure(Helper::Point3D()),
        fExternal(Helper::Point3D()),
        fInternal(Helper::Point3D()),
        fTotal(Helper::Point3D())
    {}

    Particle::Particle(const Helper::Point3D & position, double radius) :
        radius(radius),
        density(0.0),
        pressure(0.0),
        mass(0.0),
        // supportRadius(0.0),
        position(position),
        velocity(Helper::Point3D()),
        acceleration(Helper::Point3D()),
        fGravity(Helper::Point3D()),
        // fSurfaceTension(Helper::Point3D()),
        fViscosity(Helper::Point3D()),
        fPressure(Helper::Point3D()),
        fExternal(Helper::Point3D()),
        fInternal(Helper::Point3D()),
        fTotal(Helper::Point3D())
    {}
}