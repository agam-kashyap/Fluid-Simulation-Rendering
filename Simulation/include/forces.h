// Contains the code for application of forces and computation of density

#ifndef FORCES
#define FORCES

#include "particle.h"
#include "config.h"

namespace SPH
{
    class Forces
    {
        public:
            static void ComputeAllForces(ParticleVec& particleVec);
        private:
            static void ComputeDensity(ParticleVec& particleVec);
            static void ComputePressure(ParticleVec& particleVec);
            static void ComputeInternalForces(ParticleVec& particleVec);
    };
}


#endif