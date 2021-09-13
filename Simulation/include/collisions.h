#ifndef COLLISION
#define COLLISION

#include "particle.h"
#include "define.h"

#include <functional>

namespace SPH
{
    class Collision
    {
        public:
            static void detectCollisions(ParticleVec& particleVec, const Helper::Volume& volume, const std::function<float(float, float, float)>* obstacle = nullptr);
    };
}

#endif