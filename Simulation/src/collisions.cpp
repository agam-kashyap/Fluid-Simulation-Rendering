#include "../include/collisions.h"

#include "../include/config.h"
#include <Volume.h>

namespace SPH
{
    static double calculateF(const Helper::Point3D& differenceParticleNeighbour)
    {
        return differenceParticleNeighbour.calcNormSqr() - 4*Config::ParticleRadius * Config::ParticleRadius;
    }

    // (Formula 4.36)
    static Helper::Point3D calculateContactPoint(const Helper::Point3D& particlePosition,
                                                        const Helper::Point3D& differenceParticleNeighbour)
    {
        const double particleDistance = differenceParticleNeighbour.calcNorm();
        return particlePosition - (differenceParticleNeighbour / particleDistance) *(Config::ParticleRadius-particleDistance);
    }

    // (Formula 4.38)
    static Helper::Point3D calculateSurfaceNormal(const Helper::Point3D& differenceParticleNeighbour)
    {
        double particleDistance = differenceParticleNeighbour.calcNorm();
        if(particleDistance == 0) 
            particleDistance=Config::ParticleRadius/2;
        return -differenceParticleNeighbour / particleDistance;
    }

    // (Formula 4.56)
    static Helper::Point3D calculateVelocity(const Helper::Point3D& particleVelocity,
                                                    const Helper::Point3D& differenceParticleNeighbour)
    {
        const double scalarProduct = particleVelocity.x * differenceParticleNeighbour.x +
                                    particleVelocity.y * differenceParticleNeighbour.y +
                                    particleVelocity.z * differenceParticleNeighbour.z;
        return particleVelocity - differenceParticleNeighbour * (1 + Config::Elasticity) * scalarProduct;
    }
    void Collision::detectCollisions(ParticleVec& particleVec, const Helper::Volume& volume, const std::function<float(float, float, float)>* obstacle)
    {
        for (size_t i = 0; i < particleVec.size(); i++)
        {
            /* Particle Collision */
            // std::cout << particleVec[i].neighbours.size() << std::endl;
            for (size_t j = 0; j < particleVec[i].neighbours.size(); j++)
            {
                if(i==particleVec[i].neighbours[j])continue;
                Helper::Point3D differenceParticleNeighbour =
                    particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;
               
                if (calculateF(differenceParticleNeighbour) < 0)
                {
                    const Helper::Point3D surfaceNormal = calculateSurfaceNormal(differenceParticleNeighbour);

                    particleVec[i].position = calculateContactPoint(particleVec[i].position, differenceParticleNeighbour);
                    particleVec[i].velocity = calculateVelocity(particleVec[i].velocity, surfaceNormal);
                }
            }

            const Helper::Cuboid cuboid = volume.getBBox();

            if (particleVec[i].position.x > cuboid.width - particleVec[i].radius)
            {
                particleVec[i].position.x = cuboid.width - particleVec[i].radius;
                particleVec[i].velocity.x *= Config::Elasticity;
            }

            if (particleVec[i].position.x < particleVec[i].radius)
            {
                particleVec[i].position.x = particleVec[i].radius;
                particleVec[i].velocity.x *= Config::Elasticity;
            }

            if (particleVec[i].position.y > cuboid.length - particleVec[i].radius)
            {
                particleVec[i].position.y = cuboid.length - particleVec[i].radius;
                particleVec[i].velocity.y *= Config::Elasticity;
            }

            if (particleVec[i].position.y < particleVec[i].radius)
            {
                particleVec[i].position.y = particleVec[i].radius;
                particleVec[i].velocity.y *= Config::Elasticity;
            }

            if (particleVec[i].position.z > cuboid.height - particleVec[i].radius)
            {
                particleVec[i].position.z = cuboid.height - particleVec[i].radius;
                particleVec[i].velocity.z *= Config::Elasticity;
            }

            if (particleVec[i].position.z < particleVec[i].radius)
            {
                particleVec[i].position.z = particleVec[i].radius;
                particleVec[i].velocity.z *= Config::Elasticity;
            }
        }
    }
}