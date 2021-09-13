#include "../include/sph.h"

#include "../include/collisions.h"
#include "../include/config.h"
#include "../include/forces.h"
// #include "Integrator.h"


#include <cfloat>
#include <cmath>
#include <iostream>

#define PI 3.14

namespace SPH
{
    inline Helper::Point3D SpericalToCartesian(double r, double fi, double teta)
    {
        return Helper::Point3D(r * sin(teta) * cos(fi) + 1.5, r * sin(teta) * sin(fi) + 1.5, r * cos(teta) + 2.);
    }
    // Define the initial state of the particles including
    // Velocity
    // Position
    // Mass
    // Support Radius
    Simulation::Simulation(const std::function<float(float, float, float)>* obstacle):
        particles(Config::ParticleCount),
        SimulationVolume(Helper::Volume(Helper::Cuboid(Helper::Point3D(), Config::BoxWidth, Config::BoxHeight, Config::BoxDepth))),
        //neighbourhood searcher
        octree(),
        params(),
        SimulationObstacle(obstacle)
        {
            double r = 2 * Config::ParticleRadius;
            double fi = 0.;
            double teta = 0.;

            size_t M = 5;
            size_t N = 5;

            size_t m = 0;
            size_t n = 0;
        
            for(size_t i = 0u; i < Config::ParticleCount; i++)
            {
                particles[i] = SpericalToCartesian(r, fi, teta);
                particles[i].velocity = Config::InitVelocity;
                particles[i].mass = Config::WaterParticleMass;

                ++n;

                fi = 2 * PI * n / N;
                teta = PI * m / M;

                if (n == N)
                {
                    ++m;
                    n = 0;
                }

                if (m == M)
                {
                    n = 0;
                    m = 0;
                    r += 2 * Config::ParticleRadius;
                    M += 2;
                    N += 2;
                }
            }
            octree.initialize(particles);
        }
    
    void Simulation::Run()
    {
        // Search for neighbours and update it
        for(size_t i=0u; i < Config::ParticleCount; i++)
        {
            octree.radiusNeighbors<unibn::L2Distance<Particle>>(particles[i], Config::SupportRadius, particles[i].neighbours);
        }

        Forces::ComputeAllForces(particles);
        //Integrate -> Advect the particles

        Collision::detectCollisions(particles, SimulationVolume, SimulationObstacle);
    }
}