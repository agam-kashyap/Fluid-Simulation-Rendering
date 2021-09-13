// This file configures the environment properties

// #include "point.h"
#ifndef CONFIG
#define CONFIG

#include <point.h>

namespace SPH
{
    struct Config
    {
        static const size_t ParticleCount;
        static const double ParticleRadius;
        static const double Elasticity;
        static const Helper::Point3D InitVelocity;

        static const double WaterParticleMass;
        static const double WaterStiffness;
        static const double WaterDensity;
        static const double SupportRadius; //~h -> Kernel Support Radius -> Needed for ensuring Compact support condition

        //Simulation Condition
        static const double BoxWidth;
        static const double BoxHeight;
        static const double BoxDepth;
        static const Helper::Point3D InitGravitationalAcceleration;
        static Helper::Point3D GravitationalAcceleration;

        static const double SpeedThreshold;
    };
}
#endif