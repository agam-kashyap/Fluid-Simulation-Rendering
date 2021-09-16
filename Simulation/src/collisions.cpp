#include "../include/collisions.h"

#include "../include/config.h"
#include <Volume.h>

namespace SPH
{
    static Helper::Point3D collisionPoint(Helper::Point3D particlePos, Helper::Point3D directionVec)
    {
        return particlePos - (directionVec/directionVec.calcNorm()) * Config::ParticleRadius;
    }

    static double dotProduct(Helper::Point3D vecA, Helper::Point3D vecB)
    {
        return vecA.x*vecB.x + vecA.y*vecB.y + vecA.z*vecB.z;
    }

    static bool isIntersecting(Helper::Point3D vecA, Helper::Point3D vecB)
    {
        double dist = (vecA - vecB).calcNorm();
        if(std::abs(dist) < 2*Config::ParticleRadius) return true;
        else return false;
    }
    void Collision::detectCollisions(ParticleVec& particleVec, const Helper::Volume& volume, const std::function<float(float, float, float)>* obstacle)
    {
        for(size_t i=0; i< particleVec.size(); i++)
        {
            // Particle Particle Collision
            for(size_t j=0; j< particleVec[i].neighbours.size(); j++)
            {
                if(isIntersecting(particleVec[particleVec[i].neighbours[j]].position, particleVec[i].position))
                {
                    Helper::Point3D directionVec = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;
                    
                    particleVec[i].velocity = -0.5 * (1 + Config::Elasticity) * dotProduct(particleVec[i].velocity - particleVec[particleVec[i].neighbours[j]].velocity,
                                                                                directionVec) * directionVec/ directionVec.calcNormSqr();
                    particleVec[i].position = collisionPoint(particleVec[i].position, directionVec);
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
    }
}