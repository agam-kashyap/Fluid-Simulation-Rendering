#include <config.h>
#include <sph.h>
#include <Obstacles.h>

int main()
{
    SPH::Simulation sph;

    static const std::function<float(float, float, float)> obstacle = Helper::Shapes::Pawn;
    
    // sph = SPH::Simulation(&obstacle);
    for(int i=0; i< 10; i++)
    {
        std::cout << i << std::endl;
        sph = SPH::Simulation(&obstacle);
        sph.Run();
    }
}