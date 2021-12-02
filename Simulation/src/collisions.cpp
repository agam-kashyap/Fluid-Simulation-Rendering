#include "../include/collisions.h"

#include "../include/config.h"
#include <Volume.h>

namespace SPH
{
    // static Helper::Point3D collisionPoint(Helper::Point3D directionVec)
    // {
    //     return (directionVec/directionVec.calcNorm()) * (2*Config::ParticleRadius-directionVec.calcNorm());
    // }

    // static double dotProduct(Helper::Point3D vecA, Helper::Point3D vecB)
    // {
    //     return vecA.x*vecB.x + vecA.y*vecB.y + vecA.z*vecB.z;
    // }

    // static bool isIntersecting(Helper::Point3D vecA, Helper::Point3D vecB)
    // {
    //     double dist = (vecA - vecB).calcNormSqr();
    //     if(dist < 4*Config::ParticleRadius*Config::ParticleRadius)
    //     {
    //         return true;
    //     }
    //     else return false;
    // }
    // void Collision::detectCollisions(ParticleVec& particleVec, const Helper::Volume& volume, const std::function<float(float, float, float)>* obstacle)
    // {
    //     for(size_t i=0; i< particleVec.size(); i++)
    //     {
    //         // Particle Particle Collision
    //         particleVec[i].predicted_velocity = Helper::Point3D(0,0,0);
    //         int NumIntersecting = 0;
    //         for(size_t j=0; j< particleVec[i].neighbours.size(); j++)
    //         {
    //             if(i == particleVec[i].neighbours[j])continue;
    //             if(isIntersecting(particleVec[particleVec[i].neighbours[j]].position, particleVec[i].position))
    //             {
    //                 NumIntersecting++;
    //                 Helper::Point3D directionVec = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;
    //                 Helper::Point3D relativeVelocity = particleVec[i].velocity - particleVec[particleVec[i].neighbours[j]].velocity;
    //                 particleVec[i].predicted_velocity += particleVec[particleVec[i].neighbours[j]].mass * (1 + Config::Elasticity) * dotProduct(relativeVelocity, directionVec) * directionVec/(directionVec.calcNormSqr()); 
    //                 // particleVec[i].newposition += collisionPoint(directionVec);
    //             }
    //         }
    //         // std::cout << NumIntersecting << " ";
    //         particleVec[i].predicted_velocity = particleVec[i].predicted_velocity/(particleVec[i].mass * particleVec.size());
    //         // Particle Boundary Collision
    //         // const Helper::Cuboid cuboid = volume.getBBox();

    //         // if (particleVec[i].position.x > cuboid.width - particleVec[i].radius)
    //         // {
    //         //     particleVec[i].newposition.x -= particleVec[i].position.x - particleVec[i].radius;
    //         //     particleVec[i].predicted_velocity.x *= -1*Config::Elasticity;
    //         // }

    //         // if (particleVec[i].position.x < particleVec[i].radius)
    //         // {
    //         //     particleVec[i].newposition.x += particleVec[i].radius - particleVec[i].position.x;
    //         //     particleVec[i].predicted_velocity.x *= -1*Config::Elasticity;
    //         // }

    //         // if (particleVec[i].position.y > cuboid.length - particleVec[i].radius)
    //         // {
    //         //     particleVec[i].newposition.y -= particleVec[i].position.y - particleVec[i].radius;
    //         //     particleVec[i].predicted_velocity.y *= -1*Config::Elasticity;
    //         // }

    //         // if (particleVec[i].position.y < particleVec[i].radius)
    //         // {
    //         //     particleVec[i].newposition.y += particleVec[i].radius - particleVec[i].position.y;
    //         //     particleVec[i].predicted_velocity.y *= -1*Config::Elasticity;
    //         // }

    //         // if (particleVec[i].position.z > cuboid.height - particleVec[i].radius)
    //         // {
    //         //     particleVec[i].newposition.z -= particleVec[i].position.z - particleVec[i].radius;
    //         //     particleVec[i].predicted_velocity.z *= -1*Config::Elasticity;
    //         // }

    //         // if (particleVec[i].position.z < particleVec[i].radius)
    //         // {
    //         //     particleVec[i].newposition.z += particleVec[i].radius - particleVec[i].position.z;
    //         //     particleVec[i].predicted_velocity.z *= -1*Config::Elasticity;
    //         // }

    //         // Particle Obstacle Collision
    //         // if (obstacle != nullptr &&
    //         //     (*obstacle)(static_cast<float>(particleVec[i].position.x), static_cast<float>(particleVec[i].position.y),
    //         //                 static_cast<float>(particleVec[i].position.z)) > 0.f)
    //         // {
    //         //     particleVec[i].position = particleVec[i].prev_position;
    //         //     particleVec[i].velocity = particleVec[i].velocity*(-1)*Config::Elasticity;
    //         // }
    //         const Helper::Cuboid cuboid = volume.getBBox();

    //         if (particleVec[i].position.x > cuboid.width - particleVec[i].radius)
    //         {
    //             particleVec[i].position.x = cuboid.width - particleVec[i].radius;
    //             particleVec[i].predicted_velocity.x *= -1*Config::Elasticity;
    //         }

    //         if (particleVec[i].position.x < particleVec[i].radius)
    //         {
    //             particleVec[i].position.x = particleVec[i].radius ;
    //             particleVec[i].predicted_velocity.x *= -1*Config::Elasticity;
    //         }

    //         if (particleVec[i].position.y > cuboid.length - particleVec[i].radius)
    //         {
    //             particleVec[i].position.y = cuboid.length - particleVec[i].radius;
    //             particleVec[i].predicted_velocity.y *= -1*Config::Elasticity;
    //         }

    //         if (particleVec[i].position.y < particleVec[i].radius)
    //         {
    //             particleVec[i].position.y = particleVec[i].radius;
    //             particleVec[i].predicted_velocity.y *= -1*Config::Elasticity;
    //         }

    //         if (particleVec[i].position.z > cuboid.height - particleVec[i].radius)
    //         {
    //             particleVec[i].position.z = cuboid.height - particleVec[i].radius;
    //             particleVec[i].predicted_velocity.z *= -1*Config::Elasticity;
    //         }

    //         if (particleVec[i].position.z < particleVec[i].radius)
    //         {
    //             particleVec[i].position.z = particleVec[i].radius;
    //             particleVec[i].predicted_velocity.z *= -1*Config::Elasticity;
    //         }
    //     }
    //     #pragma omp parallel for
    //     for(size_t i=0; i< particleVec.size(); i++)
    //     {
    //         // particleVec[i].position += particleVec[i].newposition;
    //         particleVec[i].newposition = Helper::Point3D();

    //         // const Helper::Cuboid cuboid = volume.getBBox();

    //         // if (particleVec[i].position.x > cuboid.width - particleVec[i].radius)
    //         // {
    //         //     particleVec[i].position.x = cuboid.width - particleVec[i].radius;
    //         //     particleVec[i].predicted_velocity.x *= -1*Config::Elasticity;
    //         // }

    //         // if (particleVec[i].position.x < particleVec[i].radius)
    //         // {
    //         //     particleVec[i].position.x = particleVec[i].radius ;
    //         //     particleVec[i].predicted_velocity.x *= -1*Config::Elasticity;
    //         // }

    //         // if (particleVec[i].position.y > cuboid.length - particleVec[i].radius)
    //         // {
    //         //     particleVec[i].position.y = cuboid.length - particleVec[i].radius;
    //         //     particleVec[i].predicted_velocity.y *= -1*Config::Elasticity;
    //         // }

    //         // if (particleVec[i].position.y < particleVec[i].radius)
    //         // {
    //         //     particleVec[i].position.y = particleVec[i].radius;
    //         //     particleVec[i].predicted_velocity.y *= -1*Config::Elasticity;
    //         // }

    //         // if (particleVec[i].position.z > cuboid.height - particleVec[i].radius)
    //         // {
    //         //     particleVec[i].position.z = cuboid.height - particleVec[i].radius;
    //         //     particleVec[i].predicted_velocity.z *= -1*Config::Elasticity;
    //         // }

    //         // if (particleVec[i].position.z < particleVec[i].radius)
    //         // {
    //         //     particleVec[i].position.z = particleVec[i].radius;
    //         //     particleVec[i].predicted_velocity.z *= -1*Config::Elasticity;
    //         // }
    //         particleVec[i].velocity = particleVec[i].predicted_velocity;
    //         particleVec[i].position += particleVec[i].velocity *Config::timestep;
    //     }

    // }
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