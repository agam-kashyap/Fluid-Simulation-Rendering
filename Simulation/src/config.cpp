#include "../include/config.h"

namespace SPH
{
    const size_t Config::ParticleCount = 800;
    const double Config::ParticleRadius = 0.05;
    const double Config::Elasticity = 0.5;
    const Helper::Point3D Config::InitVelocity (0.0,0.0,0.0);

    const double Config::WaterParticleMass = 0.52;
    const double Config::WaterStiffness = 3.0; //Figure out it's value.
    const double Config::WaterDensity = 999.0;

    const double Config::CompressionControl = 1.0;
    const double Config::PressureIters = 2;
    const double Config::RelaxationFactor = 0.5;

    const double Config::SupportRadius = 0.5;
    const double Config::SmootheningLength = 1.0;
    const double Config::ScalingParam = 0.004;
    //Simulation Constraints
    const double Config::BoxWidth = 3.0;
    const double Config::BoxHeight = 3.0;
    const double Config::BoxDepth = 3.0;
    const Helper::Point3D Config::InitGravitationalAcceleration(0.0, 0.0, -9.82);
    Helper::Point3D Config::GravitationalAcceleration(0.0, 0.0, -9.82);

    const double Config::SpeedThreshold = 1.0;
}