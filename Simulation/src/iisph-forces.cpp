#include "../include/iisph-forces.h"
#include <Log.hpp>

#define _USE_MATH_DEFINES
#include <cfloat>
#include <math.h>
#include <cassert>
#include <omp.h>

#include <cstdlib>
#include <ctime>


namespace SPH
{
    static const double PI_hpow9 = M_PI * pow(Config::SupportRadius, 9);
    static const double Polynomial_Kernel_Coeff = 315.0 / (64.0 * PI_hpow9); //Made from polynomial kernel
    static const double SpikyKernel_Coeff = 45/(M_PI * pow(Config::SupportRadius, 6));
    static const double Grad_Poly_Kernel_Coeff = 945/(32 * PI_hpow9);

    static double PolyKernel(const Helper::Point3D& diffParticleNeighbour)
    {
        const double particleDistSqr = diffParticleNeighbour.calcNormSqr();
        if(diffParticleNeighbour.calcNorm() <= Config::SupportRadius)return Polynomial_Kernel_Coeff * pow((Config::SupportRadius*Config::SupportRadius - particleDistSqr), 3);
        else return 0;
    }
    static Helper::Point3D PolyKernelGradient(const Helper::Point3D diffParticleNeighbour)
    {
        return -1 * Grad_Poly_Kernel_Coeff * diffParticleNeighbour * pow((Config::SupportRadius * Config::SupportRadius - diffParticleNeighbour.calcNormSqr()),2);
    }

    static Helper::Point3D SpikyKernelGradient(const Helper::Point3D diffParticleNeighbour)
    {
        Helper::Point3D unitVec = diffParticleNeighbour/diffParticleNeighbour.calcNorm();
        return -1*SpikyKernel_Coeff*unitVec*pow(Config::SupportRadius - diffParticleNeighbour.calcNorm(), 2);
    }

    // ------------------------------- Predict Advection ---------------------------------------------
    void IISPHForces::compute_density(ParticleVec& particleVec)
    { 
        for(size_t i=0; i < particleVec.size(); i++)
        {
            double densityVal = 0;
            #pragma omp parallel for 
            for(size_t j = 0; j < particleVec[i].neighbours.size(); j++)
            {
                // if(i == particleVec[i].neighbours[j])continue;
                const Helper::Point3D diffParticleNeighbour = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;

                if(Config::SupportRadius - diffParticleNeighbour.calcNorm() > DBL_EPSILON)
                {
                    densityVal += particleVec[particleVec[i].neighbours[j]].mass * PolyKernel(diffParticleNeighbour);
                }
            }
            particleVec[i].density = densityVal;
        }
    }
    void IISPHForces::compute_gravity_force(ParticleVec& particleVec)
    {
        #pragma omp parallel for 
        for(auto& particle: particleVec)
        {
            particle.fGravity = Config::GravitationalAcceleration * particle.mass;
            // particle.fGravity = Config::GravitationalAcceleration * particle.density; 
        }
    }
    void IISPHForces::compute_advection_forces(ParticleVec& particleVec)
    {
        #pragma omp parallel for 
        for(auto& particle: particleVec)
        {
            particle.fAdvection = particle.fGravity;
        }
    }
    void IISPHForces::compute_predicted_velocity(ParticleVec& particleVec)
    {
        #pragma omp parallel for 
        for(auto& particle: particleVec)
        {
            particle.predicted_velocity = particle.velocity + Config::timestep*particle.fAdvection/particle.mass;
            // if(std::isnan(particle.velocity.x) || std::isnan(particle.velocity.y) || std::isnan(particle.velocity.z))std::cout << " ii -> " << " " << particle.neighbours.size();
        }
    }
    void IISPHForces::compute_DII(ParticleVec& particleVec)
    {
        #pragma omp parallel for 
        for(size_t i=0; i < particleVec.size(); i++)
        {
            Helper::Point3D finalDII = Helper::Point3D(0,0,0);
            #pragma omp parallel for 
            for(size_t j=0; j< particleVec[i].neighbours.size(); j++)
            {
                if(i == particleVec[i].neighbours[j])continue;
                const Helper::Point3D diffParticleNeighbour = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;
                if(Config::SupportRadius - diffParticleNeighbour.calcNorm() > DBL_EPSILON && diffParticleNeighbour.calcNorm()!=0)
                {
                    finalDII += -1 * Config::timestep * Config::timestep * particleVec[particleVec[i].neighbours[j]].mass * PolyKernelGradient(diffParticleNeighbour)/(particleVec[i].density*particleVec[i].density); 
                }
            }
            if(std::isnan(finalDII.x) || std::isnan(finalDII.y) || std::isnan(finalDII.z)) std::cout << i << std::endl;
            particleVec[i].dii = finalDII;
        }
    }
    void IISPHForces::compute_predicted_density(ParticleVec& pV)
    {
        for(size_t i=0; i < pV.size(); i++)
        {
            pV[i].predicted_density  = pV[i].density;
            for(size_t j=0; j< pV[i].neighbours.size(); j++)
            {
                if(i == pV[i].neighbours[j])continue;
                const Helper::Point3D diffParticleNeighbour = pV[i].position - pV[pV[i].neighbours[j]].position;

                #pragma omp parallel for 
                if(Config::SupportRadius - diffParticleNeighbour.calcNorm() > DBL_EPSILON && diffParticleNeighbour.calcNorm()!=0)
                {
                    pV[i].predicted_density +=  Config::timestep* pV[pV[i].neighbours[j]].mass * (pV[i].predicted_velocity - pV[pV[i].neighbours[j]].predicted_velocity).dot(PolyKernelGradient(diffParticleNeighbour)); 
                }
            }
        }
    }
    void IISPHForces::computeAii(ParticleVec& pV)
    {
        for(size_t i=0; i < pV.size(); i++)
        {
            pV[i].curr_iterate_pressure = 0.5 * pV[i].pressure;
            double aiiSUM = 0;
            for(size_t j=0; j< pV[i].neighbours.size(); j++)
            {
                if(i == pV[i].neighbours[j])continue;
                const Helper::Point3D diffParticleNeighbour = pV[i].position - pV[pV[i].neighbours[j]].position;

                #pragma omp parallel for 
                if(Config::SupportRadius - diffParticleNeighbour.calcNorm() > DBL_EPSILON && diffParticleNeighbour.calcNorm()!=0)
                {
                    Helper::Point3D dji = -1 * Config::timestep * Config::timestep *( pV[pV[i].neighbours[j]].mass/pow(pV[pV[i].neighbours[j]].density,2)) * PolyKernelGradient(-1 * diffParticleNeighbour);
                    
                    aiiSUM += pV[pV[i].neighbours[j]].mass *(pV[i].dii - dji).dot(PolyKernelGradient(diffParticleNeighbour));
                }
            }
            pV[i].aii = aiiSUM;
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
        compute_predicted_density(particleVec);
        computeAii(particleVec);
    }

    //--------------------Pressure Solve----------------------------------------------------
    void IISPHForces::computeSumDIJ(ParticleVec& particleVec, int i)
    {
        Helper::Point3D sumDIJ = Helper::Point3D(0,0,0);

        #pragma omp parallel for 
        for(size_t j=0; j< particleVec[i].neighbours.size(); j++)
        {
            if(i == particleVec[i].neighbours[j])continue;
            particleVec[particleVec[i].neighbours[j]].prev_iterate_pressure = particleVec[particleVec[i].neighbours[j]].curr_iterate_pressure;

            const Helper::Point3D diffParticleNeighbour = particleVec[i].position - particleVec[particleVec[i].neighbours[j]].position;
            if(!diffParticleNeighbour.calcNorm())continue;
            sumDIJ += -1 * particleVec[particleVec[i].neighbours[j]].mass * particleVec[particleVec[i].neighbours[j]].prev_iterate_pressure * (PolyKernelGradient(diffParticleNeighbour))/(pow(particleVec[particleVec[i].neighbours[j]].density,2));
        }
        particleVec[i].sigma_dij = Config::timestep * Config::timestep * sumDIJ;
    }
    void IISPHForces::pressureSolve(ParticleVec& pV)
    {
        int l=0;
        while(ErrorDensity(pV) > Config::CompressionControl || l < Config::PressureIters)
        {
            for(size_t i = 0; i< pV.size(); i++)
            {
                pV[i].prev_iterate_pressure = pV[i].curr_iterate_pressure;
                computeSumDIJ(pV, i); // ---->This Step computes the sigma d_ij*p_j^l
            }

            for(size_t i=0; i<pV.size(); i++)
            {
                double corr_density=0.0;
                
                #pragma omp parallel for 
                for(size_t j=0; j< pV[i].neighbours.size(); j++)
                {
                    if(i==pV[i].neighbours[j])continue;
                    const Helper::Point3D diffParticleNeighbour = pV[pV[i].neighbours[j]].position - pV[i].position; 
                    if(!diffParticleNeighbour.calcNorm())continue;
                    Helper::Point3D dji = -1 * Config::timestep * Config::timestep * (pV[i].mass/(pV[i].density*pV[i].density))*PolyKernelGradient(diffParticleNeighbour);
        
                    corr_density += pV[pV[i].neighbours[j]].mass * (pV[i].sigma_dij - (pV[pV[i].neighbours[j]].dii * pV[pV[i].neighbours[j]].prev_iterate_pressure) - (pV[pV[i].neighbours[j]].sigma_dij - dji*pV[i].pressure)).dot(PolyKernelGradient(-1*diffParticleNeighbour));

                }
                pV[i].curr_iterate_pressure = std::max((1 - Config::RelaxationFactor)*pV[i].prev_iterate_pressure +
                                                        (Config::RelaxationFactor / pV[i].aii) *
                                                        (Config::WaterDensity - pV[i].predicted_density - corr_density),0.0);
                                                        // (pV[i].density - pV[i].predicted_density - corr_density);
                pV[i].pressure = pV[i].curr_iterate_pressure;
            }
            l++;
        }
    }
    double IISPHForces::ErrorDensity(ParticleVec& particleVec)
    {
        double avg_density=0;
        #pragma omp parallel for
        for(size_t i=0; i< particleVec.size(); i++)
        {
            avg_density += particleVec[i].corrected_density ;
        }
        return avg_density/particleVec.size() - Config::WaterDensity;
    }
    //-------------------------------Integrate IISPHForces and change velocity and Advect-------------------------------
    Helper::Point3D compute_pressure_force(ParticleVec& pV, int i)
    {
        Helper::Point3D forceSum = Helper::Point3D(0,0,0);
        #pragma omp parallel for
        for(size_t j=0; j < pV[i].neighbours.size(); j++)
        {
            if(i == pV[i].neighbours[j])continue;
            const Helper::Point3D diffParticleNeighbour = pV[i].position - pV[pV[i].neighbours[j]].position;
            if(!diffParticleNeighbour.calcNorm())continue;
            forceSum += pV[pV[i].neighbours[j]].mass * ((pV[i].pressure/pow(pV[i].density,2)) + (pV[pV[i].neighbours[j]].pressure/pow(pV[pV[i].neighbours[j]].density,2))) * PolyKernelGradient(diffParticleNeighbour);
        }

        return forceSum * -1 * pV[i].mass;
    }
    void IISPHForces::integration(ParticleVec& particleVec)
    {
        #pragma omp parallel for
        for(size_t i=0; i < particleVec.size(); i++)
        {
            particleVec[i].velocity = particleVec[i].predicted_velocity + Config::timestep * compute_pressure_force(particleVec, i)/particleVec[i].mass ;
            // if(std::isnan(particleVec[i].predicted_velocity.x) || std::isnan(particleVec[i].predicted_velocity.y) || std::isnan(particleVec[i].predicted_velocity.z))std::cout << " ii -> "<< i << " " << particleVec[i].neighbours.size();
        }
        for(size_t i=0; i< particleVec.size(); i++)
        {
            particleVec[i].position += Config::timestep * particleVec[i].velocity;
        }
    }
}