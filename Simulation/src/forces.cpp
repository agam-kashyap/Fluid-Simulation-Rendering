#include "../include/forces.h"

#define _USE_MATH_DEFINES
#include <cfloat>
#include <math.h>
#include <cassert>

namespace SPH {
    static const double PI_hpow9 = M_PI * pow(Config::SmootheningLength, 9);
    static const double Kernel_Coeff = 315.0 / (64.0 * PI_hpow9); //Made from polynomial kernel
    static const double SpikyKernel_Coeff = 45/(M_PI * pow(Config::SupportRadius, 6));

    static double defaultKernel(const Helper::Point3D& diffParticleNeighbour)
    {
        const double particleDistSqr = diffParticleNeighbour.calcNormSqr();
        return Kernel_Coeff * pow((Config::SupportRadius*Config::SupportRadius - particleDistSqr), 3);
    }
    static Helper::Point3D pressureKernelGradient(const Helper::Point3D diffParticleNeighbour)
    {
        const double particleDist = diffParticleNeighbour.calcNorm();
        return SpikyKernel_Coeff * diffParticleNeighbour /particleDist
                                 * (Config::SupportRadius - particleDist)
                                 * (Config::SupportRadius - particleDist) ;
    }
    void Forces::ComputeDensity(ParticleVec& particleVec)
    {
        for(size_t i=0; i < particleVec.size(); i++)
        {
            for(size_t j = 0; j < particleVec[i].neighbours.size(); j++)
            {
                const Helper::Point3D diffParticleNeighbour = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;

                if(Config::SupportRadius - diffParticleNeighbour.calcNorm() > DBL_EPSILON)
                {
                    particleVec[i].density = Config::WaterParticleMass* defaultKernel(diffParticleNeighbour);
                    if(particleVec[i].density <= 0.0)std::cout << std::to_string(particleVec[i].density) << " " << std::to_string(i)<<","<<std::to_string(j) << std::endl;
                }
            }
        }
    }

    void Forces::ComputePressure(ParticleVec& particleVec)
    {
        for(size_t i=0; i< particleVec.size(); i++)
        {
            particleVec[i].pressure = Config::WaterStiffness * ((particleVec[i].density/Config::WaterDensity)-1);
        }
    }

    void Forces::ComputeInternalForces(ParticleVec& particleVec)
    {
        for(size_t i=0; i < particleVec.size(); i++)
        {
            particleVec[i].fPressure = Helper::Point3D();
            particleVec[i].fViscosity = Helper::Point3D();
            
            for(size_t j=0; j < particleVec[i].neighbours.size(); j++)
            {
                assert(std::abs(particleVec[i].density) > 0.);
                assert(std::abs(particleVec[particleVec[i].neighbours[j]].density) > 0.);

                const Helper::Point3D diffVectorNeighbour = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;
                const double particleDist = diffVectorNeighbour.calcNorm();

                if( std::abs(particleDist) > 0.)
                {
                    particleVec[i].fPressure += pressureKernelGradient(diffVectorNeighbour)*(
                        particleVec[i].density + particleVec[particleVec[i].neighbours[j]].density)/Config::WaterParticleMass;
                }
            }

            particleVec[i].fPressure = -1 * particleVec[i].fPressure*0.5;

            particleVec[i].fInternal = particleVec[i].fPressure;
        }
    }

    void Forces::ComputeAllForces(ParticleVec& particleVec)
    {
        Forces::ComputeDensity(particleVec);
        Forces::ComputePressure(particleVec);
        Forces::ComputeInternalForces(particleVec);

        for(auto& particle : particleVec)
        {
            particle.fTotal = particle.fExternal + particle.fInternal;
        }
    }
}