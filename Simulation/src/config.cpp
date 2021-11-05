#include "../include/config.h"

namespace SPH
{
    const size_t Config::ParticleCount = 1000;
    const double Config::ParticleRadius = 0.015;
    const double Config::Elasticity = 0.4;
    const Helper::Point3D Config::InitVelocity (0.0,0.0,0.0);

    const double Config::WaterParticleMass = 0.022;
    const double Config::WaterDensity = 999.0;

    const double Config::CompressionControl = 1.0;
    const double Config::PressureIters = 2;
    const double Config::RelaxationFactor = 0.5;

    const double Config::SupportRadius = 0.1;
    const double Config::SmootheningLength = 1.5;
    const double Config::ScalingParam = 0.004;
    //Simulation Constraints
    const double Config::BoxWidth = 0.8;
    const double Config::BoxHeight = 0.8;
    const double Config::BoxDepth = 0.8;
    const Helper::Point3D Config::InitGravitationalAcceleration(0.0, 0.0, -9.82);
    Helper::Point3D Config::GravitationalAcceleration(0.0, 0.0, -9.82);

    const double Config::SpeedThreshold = 700.0;
    const double Config::timestep = 0.0005;
}