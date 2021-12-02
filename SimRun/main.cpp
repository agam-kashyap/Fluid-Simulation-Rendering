#include <config.h>
#include <sph.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

int main()
{
    SPH::Simulation* sph = new SPH::Simulation(nullptr);

    std::string fileName = "dam-12000frames-aiiSolutionTry-" + std::to_string(SPH::Config::ParticleCount) + ".r-" + std::to_string(SPH::Config::ParticleRadius) + ".txt";
    std::cout << fileName << std::endl;
    float progress = 0.0;
    int particleCount = sph->particles.size();
    int barWidth = 70;
    
    std::ofstream FluidFile (fileName); 
    FluidFile << particleCount << " " << SPH::Config::ParticleRadius << std::endl;

    for(int i=0; i< 12000; i++)
    {
        sph->Run();
        for (int i=0; i< particleCount; i++)
        {
            FluidFile << sph->particles[i].position.x << " "<< sph->particles[i].position.y << " " << sph->particles[i].position.z << std::endl; 
        }

        std::cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();

        progress += 1.0/12000; 
    }
    FluidFile.close();
    std::cout << std::endl;
}