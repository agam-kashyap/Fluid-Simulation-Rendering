#include "../include/sph.h"

#include "../include/collisions.h"
#include "../include/config.h"
#include "../include/iisph-forces.h"

#include <cfloat>
#include <cmath>
#include <iostream>
#include <fstream>
#include <time.h>

#define PI 3.14

namespace SPH
{
    inline Helper::Point3D SpericalToCartesian(double r, double fi, double teta)
    {
        return Helper::Point3D(r * sin(teta) * cos(fi) + 1.5, r * sin(teta) * sin(fi) + 1.5, r * cos(teta) + 2.);
    }
    Simulation::Simulation(const std::function<float(float, float, float)>* obstacle):
        minDensity(0),
        maxDensity(0),
        particles(Config::ParticleCount),
        SimulationVolume(Helper::Volume(Helper::Cuboid(Helper::Point3D(), Config::BoxWidth, Config::BoxHeight, Config::BoxDepth))),
        octree(),
        params(),
        SimulationObstacle(obstacle)
        {
            double x_pos = Config::ParticleRadius ;
            double y_pos = Config::ParticleRadius ;
            double z_pos = Config::ParticleRadius;

            int X_prev = 0;
            int Y_prev = 0;
            int Z_prev = 0;
            srand( (unsigned)time( NULL ) );

            for(size_t i = 0u; i < Config::ParticleCount; i++)
            {
                particles[i] = Helper::Point3D(x_pos, y_pos, z_pos);
                particles[i].velocity = Config::InitVelocity;
                particles[i].mass = Config::WaterParticleMass ;
                //-----------------Arranges in a Dam Shaped Structure -------
                x_pos += 2* Config::ParticleRadius;
                if(x_pos > 0.25)
                {
                    if(!X_prev)
                    {
                        X_prev = 1;
                        x_pos = 2*Config::ParticleRadius;
                    }
                    else
                    {
                        X_prev = 0;
                        x_pos = Config::ParticleRadius;
                    }
                    y_pos += sqrt(3)*Config::ParticleRadius;
                }
                if(y_pos > 0.25)
                {
                    if(!Z_prev)
                    {
                        Z_prev = 1;
                        X_prev = 1;
                        x_pos = 2*Config::ParticleRadius;
                        y_pos = Config::ParticleRadius*(1 + sqrt(3)/2);
                    }
                    else
                    {
                        Z_prev = 0;
                        X_prev = 0;
                        x_pos = Config::ParticleRadius;
                        y_pos = Config::ParticleRadius; 
                    }
                    z_pos += 1.5*Config::ParticleRadius;
                }
            }
            params.copyPoints = true;

            // octree.initialize(particles);
        }
    
    void Simulation::Run()
    {
        octree.initialize(particles);
        for(size_t i=0u; i < Config::ParticleCount; i++)
        {
            octree.radiusNeighbors<unibn::L2Distance<Particle>>(particles[i], Config::SupportRadius, particles[i].neighbours);
        }
        IISPHForces::predict_advection(particles); 

        float min = 1000;
        float max = -1;
        for(auto p: particles)
        {
            if(p.density > max) max = p.density;
            if(p.density < min) min = p.density;
        }  
        minDensity = min;
        maxDensity = max;

        IISPHForces::pressureSolve(particles);
        IISPHForces::integration(particles);
        IISPHForces::compute_density(particles);

        for(size_t i=0; i< Config::ParticleCount; i++)
        {
            if(std::isnan(particles[i].position.x) || std::isnan(particles[i].position.y) || std::isnan(particles[i].position.z))
            {
                std::cout << i << " -!- " << particles[i].position;
                exit(0);
            }
        }
        octree.initialize(particles);
        for(size_t i=0u; i < Config::ParticleCount; i++)
        {
            octree.radiusNeighbors<unibn::L2Distance<Particle>>(particles[i], Config::SupportRadius, particles[i].neighbours);
        }
        Collision::detectCollisions(particles, SimulationVolume, SimulationObstacle);
    }
}