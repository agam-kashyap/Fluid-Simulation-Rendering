#include "../include/sph.h"

#include "../include/collisions.h"
#include "../include/config.h"
#include "../include/forces.h"
#include "../include/iisph-forces.h"
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
        octree(),
        params(),
        SimulationObstacle(obstacle)
        {
            double x_pos = Config::ParticleRadius;// + 1.5;
            double y_pos = Config::ParticleRadius;// + 1.5;
            double z_pos = Config::ParticleRadius;// + 1.5;

            for(size_t i = 0u; i < Config::ParticleCount; i++)
            {
                // particles[i] = SpericalToCartesian(r, fi, teta);
                particles[i] = Helper::Point3D(x_pos, y_pos, z_pos);
                particles[i].velocity = Config::InitVelocity;
                particles[i].mass = Config::WaterParticleMass;
                //-----------------Arranges in a Dam Shaped Structure -------
                x_pos += 2* Config::ParticleRadius;
                if(x_pos > 1.0)
                {
                    x_pos = Config::ParticleRadius ;//+ 1.5;
                    y_pos += 2*Config::ParticleRadius;
                }
                if(y_pos > 1.0)
                {
                    y_pos = Config::ParticleRadius ;//+ 1.5;
                    z_pos += 2*Config::ParticleRadius;
                }
            }
            params.copyPoints = true;

            octree.initialize(particles);
            std::cout << "Octree initialised" << std::endl;
        }
    
    void Simulation::Run()
    {
        // Search for neighbours and update it
        std::cout << "Neighbour Search Begins" << std::endl;
        for(size_t i=0u; i < Config::ParticleCount; i++)
        {
            octree.radiusNeighbors<unibn::L2Distance<Particle>>(particles[i], Config::SupportRadius, particles[i].neighbours);
        }
        IISPHForces::predict_advection(particles);
        
        IISPHForces::pressureSolve(particles);
        IISPHForces::integration(particles);
        std::cout <<"Post Integration: " << particles[0].position << " " << std::to_string(particles.size())<< std::endl;
        Collision::detectCollisions(particles, SimulationVolume, SimulationObstacle);
    }
}