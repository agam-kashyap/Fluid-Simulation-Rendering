#include <cassert>
#include <cmath>

namespace Helper
{
    template <class T> T ROperations::conjunction(T x, T y)
    {
        return x + y - std::sqrt(x * x + y * y);
    }

    template <class T> T ROperations::disjunction(T x, T y)
    {
        return x + y + std::sqrt(x * x + y * y);
    }

}