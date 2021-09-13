#include "../include/config.h"

namespace SPH
{
    const size_t Config::ParticleCount = 600;
    const double Config::ParticleRadius = 0.015;
    const double Config::Elasticity = 0.5;
    const Helper::Point3D Config::InitVelocity (0.0,0.0,0.0);

    const double Config::WaterParticleMass = 0.02;
    const double Config::WaterStiffness = 3.0; //Figure out it's value.
    const double Config::WaterDensity = 999.0;

    const double Config::SupportRadius = 0.1;

    //Simulation Constraints
    const double Config::BoxWidth = 3.0;
    const double Config::BoxHeight = 3.0;
    const double Config::BoxDepth = 3.0;
    const Helper::Point3D Config::InitGravitationalAcceleration(0.0, 0.0, -9.82);
    Helper::Point3D Config::GravitationalAcceleration(0.0, 0.0, -9.82);

    const double Config::SpeedThreshold = 3.0;
}