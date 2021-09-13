#include "../include/collisions.h"

static Helper::Point3D collisionPoint(Helper::Point3D particlePos, Helper::Point3D directionVec)
{
    return particlePos - (directionVec/directionVec.calcNorm())*Config::ParticleRadius;
}

static double dotProduct(Helper::Point3D vecA, Helper::Point3D vecB)
{
    return vecA.x*vecB.x + vecA.y*vecB.y + vecA.z*vecB.z;
}

static bool isIntersecting(Helper::Point3D vecA, Helper::Point3D vecB)
{
    double dist = (vecA.position - vecB.position).calcNorm();
    if(std::abs(dist) < 2*Config::ParticleRadius) return true;
    else return false;
}
void Collision::detectCollisions(ParticleVec& particleVec, Helper::Volume& volume, std::function<float(float, float, float)>* obstacle=nullptr)
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

        if (particleVect[i].position.x > cuboid.width - particleVect[i].radius)
        {
            particleVect[i].position.x = cuboid.width - particleVect[i].radius;
            particleVect[i].velocity.x *= -1*Config::Elasticity;
        }

        if (particleVect[i].position.x < particleVect[i].radius)
        {
            particleVect[i].position.x = particleVect[i].radius;
            particleVect[i].velocity.x *= -1*Config::Elasticity;
        }

        if (particleVect[i].position.y > cuboid.length - particleVect[i].radius)
        {
            particleVect[i].position.y = cuboid.length - particleVect[i].radius;
            particleVect[i].velocity.y *= -1*Config::Elasticity;
        }

        if (particleVect[i].position.y < particleVect[i].radius)
        {
            particleVect[i].position.y = particleVect[i].radius;
            particleVect[i].velocity.y *= -1*Config::Elasticity;
        }

        if (particleVect[i].position.z > cuboid.height - particleVect[i].radius)
        {
            particleVect[i].position.z = cuboid.height - particleVect[i].radius;
            particleVect[i].velocity.z *= -1*Config::Elasticity;
        }

        if (particleVect[i].position.z < particleVect[i].radius)
        {
            particleVect[i].position.z = particleVect[i].radius;
            particleVect[i].velocity.z *= -1*Config::Elasticity;
        }

        // Particle Obstacle Collision
        if (obstacle != nullptr &&
            (*obstacle)(static_cast<float>(particleVec[i].position.x), static_cast<float>(particleVec[i].position.y),
                        static_cast<float>(particleVec[i].position.z)) > 0.f)
        {
            particleVec[i].position = particleVec[i].previous_position;
            particleVec[i].velocity *= -1*Config::Elasticity;
        }
    }
}