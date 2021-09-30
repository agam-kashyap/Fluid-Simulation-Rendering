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
            double curr_iterate_pressure;
            double prev_iterate_pressure;
            double mass;

            Helper::Point3D dii;
            double aii;
            double predicted_density; //rho_adv
            double corrected_density; //sum of rho_adv + extra term in eq 13
            Helper::Point3D sigma_dij; // Sigma dij for all neighbours 
            Helper::Point3D position;
            Helper::Point3D prev_position;
            Helper::Point3D velocity;
            Helper::Point3D predicted_velocity;
            Helper::Point3D acceleration;

            Helper::Point3D fGravity;
            Helper::Point3D fPressure;
            Helper::Point3D fViscosity;
            Helper::Point3D fAdvection;

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