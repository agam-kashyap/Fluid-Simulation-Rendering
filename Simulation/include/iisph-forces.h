#ifndef IISPH_FORCES
#define IISPH_FORCES

#include "particle.h"
#include "config.h"

namespace SPH
{
    class IISPHForces
    {
        public:
            static void predict_advection(ParticleVec& particleVec);
            static void pressureSolve(ParticleVec& particleVec);
            static void integration(ParticleVec& particleVec);
            static void compute_density(ParticleVec& particleVec);
        private:
            static void compute_gravity_force(ParticleVec& particleVec);
            // static void compute_viscous_force(ParticleVec& particleVec);
            // static void compute_density(ParticleVec& particleVec);
            static void compute_predicted_velocity(ParticleVec& particleVec);
            static void compute_advection_forces(ParticleVec& particleVec);
            static void compute_DII(ParticleVec& particleVec);
            static void compute_predicted_density(ParticleVec& particleVec);
            static void computeAii(ParticleVec& particleVec);

            static void computeSumDIJ(ParticleVec& particleVec, int i);
            static double ErrorDensity(ParticleVec& particleVec);
    };
}
#endif