#include "../include/collisions.h"

#include "../include/config.h"
#include <Volume.h>

namespace SPH
{
    static const double timestep = Config::ParticleRadius*2 * Config::ScalingParam/Config::SpeedThreshold; //Redefine the vmax -> Think about how to decide
    static Helper::Point3D collisionPoint(Helper::Point3D particlePos, Helper::Point3D directionVec)
    {
        // std::cout << std::to_string(directionVec.x) << " " <<std::to_string(directionVec.y) <<" " << std::to_string(directionVec.z) << "::::";
        // std::cout << std::to_string(directionVec.calcNorm()) << " " << std::to_string((directionVec/directionVec.calcNorm()).x)  << std::endl;
        return particlePos + (directionVec/directionVec.calcNorm()) * Config::ParticleRadius;
    }

    static double dotProduct(Helper::Point3D vecA, Helper::Point3D vecB)
    {
        return vecA.x*vecB.x + vecA.y*vecB.y + vecA.z*vecB.z;
    }

    static bool isIntersecting(Helper::Point3D vecA, Helper::Point3D vecB)
    {
        double dist = (vecA - vecB).calcNormSqr();
        if(dist < 4*Config::ParticleRadius*Config::ParticleRadius)
        {
            // std::cout << "Point A: " << vecA << " PointB: " << vecB << " " << std::to_string(dist) << std::endl;
            return true;
        }
        else return false;
    }
    void Collision::detectCollisions(ParticleVec& particleVec, const Helper::Volume& volume, const std::function<float(float, float, float)>* obstacle)
    {
        for(size_t i=0; i< particleVec.size(); i++)
        {
            // Particle Particle Collision
            for(size_t j=0; j< particleVec[i].neighbours.size(); j++)
            {
                if(i == particleVec[i].neighbours[j])continue;
                if(isIntersecting(particleVec[particleVec[i].neighbours[j]].position, particleVec[i].position))
                {
                    Helper::Point3D directionVec = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;
                    
                    particleVec[i].velocity += -0.5 * (1 - Config::Elasticity) * dotProduct(particleVec[i].velocity - particleVec[particleVec[i].neighbours[j]].velocity,
                                                                                directionVec) * directionVec/ directionVec.calcNormSqr(); //It should be summation of all directions.
                    // particleVec[i].position = collisionPoint(particleVec[i].position, directionVec); 
                }
            }
            // Particle Boundary Collision
            const Helper::Cuboid cuboid = volume.getBBox();

            if (particleVec[i].position.x > cuboid.width - particleVec[i].radius)
            {
                particleVec[i].position.x = cuboid.width - particleVec[i].radius;
                particleVec[i].velocity.x *= -1*Config::Elasticity;
            }

            if (particleVec[i].position.x < particleVec[i].radius)
            {
                particleVec[i].position.x = particleVec[i].radius;
                particleVec[i].velocity.x *= -1*Config::Elasticity;
            }

            if (particleVec[i].position.y > cuboid.length - particleVec[i].radius)
            {
                particleVec[i].position.y = cuboid.length - particleVec[i].radius;
                particleVec[i].velocity.y *= -1*Config::Elasticity;
            }

            if (particleVec[i].position.y < particleVec[i].radius)
            {
                particleVec[i].position.y = particleVec[i].radius;
                particleVec[i].velocity.y *= -1*Config::Elasticity;
            }

            if (particleVec[i].position.z > cuboid.height - particleVec[i].radius)
            {
                particleVec[i].position.z = cuboid.height - particleVec[i].radius;
                particleVec[i].velocity.z *= -1*Config::Elasticity;
            }

            if (particleVec[i].position.z < particleVec[i].radius)
            {
                particleVec[i].position.z = particleVec[i].radius;
                particleVec[i].velocity.z *= -1*Config::Elasticity;
            }

            // Particle Obstacle Collision
            if (obstacle != nullptr &&
                (*obstacle)(static_cast<float>(particleVec[i].position.x), static_cast<float>(particleVec[i].position.y),
                            static_cast<float>(particleVec[i].position.z)) > 0.f)
            {
                particleVec[i].position = particleVec[i].prev_position;
                particleVec[i].velocity = particleVec[i].velocity*(-1)*Config::Elasticity;
            }
        }
        for(size_t i=0; i< particleVec.size(); i++)
        {
            particleVec[i].position += particleVec[i].velocity * timestep;
        }
    }
}