#ifndef R_OPERATIONS
#define R_OPERATIONS

namespace Helper
{
    /**
     * @brief ROperations class defines R-operations.
     */
    class ROperations
    {
        public:
            /**
             * @brief Returns conjunction of x and y
             * @param x    The x Cartesian coordinate
             * @param y    The y Cartesian coordinate
             * @return the result of conjuction R-operation in R0 system
             */
            template <class T> static T conjunction(T x, T y);

            /**
             * @brief Returns disjunction of x and y
             * @param x    The x Cartesian coordinate
             * @param y    The y Cartesian coordinate
             * @return the result of disjunction R-operation in R0 system
             */
            template <class T> static T disjunction(T x, T y);
    };

}

#include "ROperations.hpp"

#endif
