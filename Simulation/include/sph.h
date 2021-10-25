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
            static double get(const SPH::Particle& p)
            {
                // std::cout << "get 0 "<< std::to_string(p.position.x) << std::endl;
                return p.position.x;
            }
        };

        template <>
        struct access<SPH::Particle, 1>
        {
            static double get(const SPH::Particle& p)
            {
                // std::cout << "get 1 "<< std::to_string(p.position.y) << std::endl;
                return p.position.y;
            }
        };

        template <>
        struct access<SPH::Particle, 2>
        {
            static double get(const SPH::Particle& p)
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
        
        public:
            float minDensity;
            float maxDensity;
            Helper::Volume SimulationVolume;
            unibn::Octree<Particle> octree;
            unibn::OctreeParams params;
            const std::function<float(float, float, float)>* SimulationObstacle;
    };
}
#endif