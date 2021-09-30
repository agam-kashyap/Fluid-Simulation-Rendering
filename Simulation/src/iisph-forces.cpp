#include "../include/iisph-forces.h"

#define _USE_MATH_DEFINES
#include <cfloat>
#include <math.h>
#include <cassert>

namespace SPH
{
    static const double PI_hpow9 = M_PI * pow(Config::SmootheningLength, 9);
    static const double Polynomial_Kernel_Coeff = 315.0 / (64.0 * PI_hpow9); //Made from polynomial kernel
    // static const double SpikyKernel_Coeff = 45/(M_PI * pow(Config::SupportRadius, 6));
    static const double Grad_Poly_Kernel_Coeff = 945/(32 * PI_hpow9);

    static const double timestep = Config::ParticleRadius*2 * Config::ScalingParam/Config::SpeedThreshold; //Redefine the vmax -> Think about how to decide
    
    static double defaultKernel(const Helper::Point3D& diffParticleNeighbour)
    {
        const double particleDistSqr = diffParticleNeighbour.calcNormSqr();
        // std::cout << Polynomial_Kernel_Coeff * pow((Config::SupportRadius*Config::SupportRadius - particleDistSqr), 3) << std::endl; 
        return Polynomial_Kernel_Coeff * pow((Config::SupportRadius*Config::SupportRadius - particleDistSqr), 3);
    }
    static Helper::Point3D PolyKernelGradient(const Helper::Point3D diffParticleNeighbour)
    {
        return -1 * Grad_Poly_Kernel_Coeff * diffParticleNeighbour * pow((Config::SupportRadius * Config::SupportRadius - diffParticleNeighbour.calcNormSqr()),2);
    }

    // ------------------------------- Predict Advection ---------------------------------------------
    void IISPHForces::compute_density(ParticleVec& particleVec)
    {
        for(size_t i=0; i < particleVec.size(); i++)
        {
            for(size_t j = 0; j < particleVec[i].neighbours.size(); j++)
            {
                if(i == particleVec[i].neighbours[j])continue;
                const Helper::Point3D diffParticleNeighbour = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;

                // std::cout << defaultKernel(diffParticleNeighbour) << std::endl;
                if(Config::SupportRadius - diffParticleNeighbour.calcNorm() > DBL_EPSILON)
                {
                    particleVec[i].density += particleVec[particleVec[i].neighbours[j]].mass * defaultKernel(diffParticleNeighbour);
                    // std::cout << particleVec[i].density << std::endl;
                }
            }
            // std::cout << particleVec[i].density << std::endl;
        }
    }
    void IISPHForces::compute_gravity_force(ParticleVec& particleVec)
    {
        for(auto& particle: particleVec)
        {
            particle.fGravity = Config::GravitationalAcceleration * particle.density;
            // std::cout << particle.fGravity << std::endl;
        }
    }
    void IISPHForces::compute_advection_forces(ParticleVec& particleVec)
    {
        for(auto& particle: particleVec)
        {
            particle.fAdvection = particle.fGravity; //+ other forces;
            // std::cout << particle.fGravity << std::endl;
        }
    }
    void IISPHForces::compute_predicted_velocity(ParticleVec& particleVec)
    {
        for(auto& particle: particleVec)
        {
            particle.predicted_velocity = particle.velocity + timestep*particle.fAdvection/particle.mass;
            // std::cout << particle.predicted_velocity << std::to_string(timestep) << " " <<  particle.fAdvection << " " << std::to_string(particle.mass) << std::endl;
        }
    }
    void IISPHForces::compute_DII(ParticleVec& particleVec)
    {
        for(size_t i=0; i < particleVec.size(); i++)
        {
            for(size_t j=0; j< particleVec[i].neighbours.size(); j++)
            {
                if(i == particleVec[i].neighbours[j])continue;
                const Helper::Point3D diffParticleNeighbour = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;
                if(Config::SupportRadius - diffParticleNeighbour.calcNorm() > DBL_EPSILON)
                {
                    particleVec[i].dii += -1 * timestep * timestep * particleVec[particleVec[i].neighbours[j]].mass * PolyKernelGradient(diffParticleNeighbour)/(particleVec[i].density*particleVec[i].density); 
                }
            }
        }
    }
    void IISPHForces::computeAii(ParticleVec& particleVec)
    {
        for(size_t i=0; i < particleVec.size(); i++)
        {
            particleVec[i].predicted_density  = particleVec[i].density;
            for(size_t j=0; j< particleVec[i].neighbours.size(); j++)
            {
                if(i == particleVec[i].neighbours[j])continue;
                const Helper::Point3D diffParticleNeighbour = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;
                if(Config::SupportRadius - diffParticleNeighbour.calcNorm() > DBL_EPSILON)
                {
                    particleVec[i].predicted_density +=  timestep* particleVec[particleVec[i].neighbours[j]].mass * (particleVec[i].predicted_velocity - particleVec[particleVec[i].neighbours[j]].predicted_velocity).dot(PolyKernelGradient(diffParticleNeighbour))/(particleVec[i].density*particleVec[i].density); 
                    
                    particleVec[i].curr_iterate_pressure = 0.5 * particleVec[i].pressure;

                    Helper::Point3D dji = -1 * timestep * timestep *( particleVec[particleVec[i].neighbours[j]].mass/pow(particleVec[particleVec[i].neighbours[j]].density,2)) * particleVec[particleVec[i].neighbours[j]].pressure * PolyKernelGradient(-1 * diffParticleNeighbour);
                    // std::cout << dji << std::endl;
                    particleVec[i].aii += particleVec[particleVec[i].neighbours[j]].mass *(particleVec[i].dii - dji).dot(PolyKernelGradient(diffParticleNeighbour)); 
                    // std::cout << particleVec[i].dii << std::endl;
                }
            }
            std::cout << particleVec[i].aii << std::endl;
            // std::cout << particleVec[i].curr_iterate_pressure << std::endl;
        }
    }
    void IISPHForces::predict_advection(ParticleVec& particleVec)
    {
        // ---- This block can be combined into 1 ----
        compute_density(particleVec);
        compute_gravity_force(particleVec);
        compute_advection_forces(particleVec);
        compute_predicted_velocity(particleVec);
        compute_DII(particleVec);
        // ----- Separate block ------
        computeAii(particleVec);
    }

    //--------------------Pressure Solve----------------------------------------------------
    void IISPHForces::computeSumDIJ(ParticleVec& particleVec, int i)
    {
        for(size_t j=0; j< particleVec[i].neighbours.size(); j++)
        {
            particleVec[particleVec[i].neighbours[j]].prev_iterate_pressure = particleVec[particleVec[i].neighbours[j]].curr_iterate_pressure;

            // std::cout << particleVec[particleVec[i].neighbours[j]].curr_iterate_pressure << std::endl;

            Helper::Point3D diffParticleNeighbour = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;
            particleVec[i].sigma_dij += -1 * timestep * timestep * particleVec[particleVec[i].neighbours[j]].mass * particleVec[particleVec[i].neighbours[j]].prev_iterate_pressure * (PolyKernelGradient(diffParticleNeighbour));
        }
    }
    void IISPHForces::pressureSolve(ParticleVec& particleVec)
    {
        int l=0;
        while(ErrorDensity(particleVec) > Config::CompressionControl || l < Config::PressureIters)
        {
            for(size_t i = 0; i< particleVec.size(); i++)
            {
                particleVec[i].prev_iterate_pressure = particleVec[i].curr_iterate_pressure;
                computeSumDIJ(particleVec, i); // ---->This Step computes the sigma d_ij*p_j^l
            }
            for(size_t i=0; i<particleVec.size(); i++)
            {
                double corr_density=0.0;
                for(size_t j=0; j< particleVec[i].neighbours.size(); j++)
                {
                    Helper::Point3D diffParticleNeighbour = particleVec[particleVec[i].neighbours[j]].position - particleVec[i].position; 
                    Helper::Point3D dji = -1 * timestep * timestep * (particleVec[particleVec[i].neighbours[j]].mass/(particleVec[particleVec[i].neighbours[j]].density*particleVec[particleVec[i].neighbours[j]].density))*PolyKernelGradient(diffParticleNeighbour);
                    // std::cout << dji << std::endl;
                    corr_density += particleVec[particleVec[i].neighbours[j]].mass * (particleVec[i].sigma_dij - (particleVec[particleVec[i].neighbours[j]].dii * particleVec[particleVec[i].neighbours[j]].prev_iterate_pressure) - 
                                                                                (particleVec[particleVec[i].neighbours[j]].sigma_dij - dji)).dot(PolyKernelGradient(-1*diffParticleNeighbour));
                }
                particleVec[i].curr_iterate_pressure = (1 - Config::RelaxationFactor)*particleVec[i].prev_iterate_pressure +
                                                        (Config::RelaxationFactor / particleVec[i].aii) *
                                                        (Config::WaterDensity - particleVec[i].predicted_density - corr_density);
                // std::cout << particleVec[i].curr_iterate_pressure << std::endl;
                particleVec[i].pressure = particleVec[i].curr_iterate_pressure;
            }
            l++;
        }
    }
    double IISPHForces::ErrorDensity(ParticleVec& particleVec)
    {
        double avg_density=0;
        for(size_t i=0; i< particleVec.size(); i++)
        {
            // std::cout << particleVec[i].curr_iterate_pressure << std::endl;
            avg_density += particleVec[i].corrected_density ;
        }
        return avg_density/particleVec.size() - Config::WaterDensity;
    }
    //-------------------------------Integrate IISPHForces and change velocity and Advect-------------------------------
    Helper::Point3D compute_pressure_force(ParticleVec& pV, int i)
    {
        Helper::Point3D forceSum = Helper::Point3D();
        for(size_t j=0; j < pV[i].neighbours.size(); j++)
        {
            Helper::Point3D diffParticleNeighbour = pV[i].position - pV[pV[i].neighbours[j]].position;
            forceSum += pV[pV[i].neighbours[j]].mass * ((pV[i].pressure/pow(pV[i].density,2)) + (pV[pV[i].neighbours[j]].pressure/pow(pV[pV[i].neighbours[j]].density,2))) * PolyKernelGradient(diffParticleNeighbour);
        }
        return forceSum * -1 * pV[i].mass;
    }
    void IISPHForces::integration(ParticleVec& particleVec)
    {
        for(size_t i=0; i < particleVec.size(); i++)
        {
            particleVec[i].velocity = particleVec[i].predicted_velocity + timestep * compute_pressure_force(particleVec, i)/particleVec[i].mass ;
            // std::cout << particleVec[i].predicted_velocity << std::endl;
            particleVec[i].position = particleVec[i].position + timestep * particleVec[i].velocity;
            
        }
    }
}
/*
Remember to Update neighbours somewhere
*/