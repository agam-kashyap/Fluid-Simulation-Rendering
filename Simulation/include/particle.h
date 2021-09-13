#ifndef PARTICLE
#define PARTICLE

#include "config.h"
#include "define.h"
#include <point.h>
#include <bits/stdc++.h>

namespace SPH
{
    class Particle
    {
        public:
            Particle();
            Particle(const Helper::Point3D& position, double radius = Config::ParticleRadius);

            double radius;
            double density;
            double pressure;
            double mass;

            Helper::Point3D position;
            Helper::Point3D prev_position;
            Helper::Point3D velocity;
            Helper::Point3D acceleration;

            Helper::Point3D fGravity;
            Helper::Point3D fPressure;
            Helper::Point3D fViscosity;

            Helper::Point3D fExternal;
            Helper::Point3D fInternal;

            Helper::Point3D fTotal;

            std::vector<uint32_t> neighbours;
    };

    using ParticleVec = std::vector<Particle>;
    using ParticleVecConstIter = ParticleVec::const_iterator;
    using ParticleVecIter = ParticleVec::iterator;
    
}

#endif