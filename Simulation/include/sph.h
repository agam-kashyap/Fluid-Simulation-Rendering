#ifndef SPH_H
#define SPH_H

#include "particle.h"
#include <Volume.h>
#include <Octree.hpp>

#include <functional>

namespace unibn
{
    namespace traits
    {
        template <>
        struct access<SPH::Particle, 0>
        {
            static float get(const SPH::Particle& p)
            {
                return p.position.x;
            }
        };

        template <>
        struct access<SPH::Particle, 1>
        {
            static float get(const SPH::Particle& p)
            {
                return p.position.y;
            }
        };

        template <>
        struct access<SPH::Particle, 2>
        {
            static float get(const SPH::Particle& p)
            {
                return p.position.z;
            }
        };
    }
}

namespace SPH
{
    class Simulation
    {
        public:
            Simulation(const std::function<float(float, float, float)>* obstacle = nullptr);

            void Run();

            ParticleVec particles;
        
        private:
            Helper::Volume SimulationVolume;
            // Neighbourhood Search
            unibn::Octree<Particle> octree;
            unibn::OctreeParams params;
            const std::function<float(float, float, float)>* SimulationObstacle;
    };
}
#endif