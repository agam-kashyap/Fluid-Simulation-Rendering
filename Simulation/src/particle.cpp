#include "../include/particle.h"

namespace SPH
{
    Particle::Particle() :
        radius(0.0),
        density(0.0),
        pressure(0.0),
        mass(0.0),
        dii(Helper::Point3D()),
        aii(0.0),
        predicted_density(0.0),
        corrected_density(0.0),
        curr_iterate_pressure(0.0),
        prev_iterate_pressure(0.0),
        sigma_dij(Helper::Point3D()),
        // supportRadius(0.0),
        position(Helper::Point3D()),
        newposition(Helper::Point3D()),
        velocity(Helper::Point3D()),
        predicted_velocity(Helper::Point3D()),
        acceleration(Helper::Point3D()),
        fGravity(Helper::Point3D()),
        // fSurfaceTension(Helper::Point3D()),
        fViscosity(Helper::Point3D()),
        fPressure(Helper::Point3D()),
        fAdvection(Helper::Point3D()),
        fExternal(Helper::Point3D()),
        fInternal(Helper::Point3D()),
        fTotal(Helper::Point3D())
    {}

    Particle::Particle(const Helper::Point3D & position, double radius) :
        radius(radius),
        density(0.0),
        pressure(0.0),
        mass(0.0),
        dii(Helper::Point3D()),
        aii(0.0),
        predicted_density(0.0),
        corrected_density(0.0),
        curr_iterate_pressure(0.0),
        prev_iterate_pressure(0.0),
        sigma_dij(Helper::Point3D()),
        // supportRadius(0.0),
        position(position),
        newposition(Helper::Point3D()),
        velocity(Helper::Point3D()),
        predicted_velocity(Helper::Point3D()),
        acceleration(Helper::Point3D()),
        fGravity(Helper::Point3D()),
        // fSurfaceTension(Helper::Point3D()),
        fViscosity(Helper::Point3D()),
        fPressure(Helper::Point3D()),
        fAdvection(Helper::Point3D()),
        fExternal(Helper::Point3D()),
        fInternal(Helper::Point3D()),
        fTotal(Helper::Point3D())
    {}
}