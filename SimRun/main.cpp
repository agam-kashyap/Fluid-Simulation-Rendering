#include <config.h>
#include <sph.h>
#include <Obstacles.h>

int main()
{
    // SPH::Simulation sph;
    std::cout << "Declaration" << std::endl;

    static const std::function<float(float, float, float)> obstacle = Helper::Shapes::Pawn;
    
    SPH::Simulation* sph = new SPH::Simulation(&obstacle);

    for(int i=0; i< 10; i++)
    {
        std::cout << i << std::endl;
        sph->Run();
    }
}