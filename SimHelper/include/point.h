#ifndef POINT
#define POINT

#include <vector>
namespace Helper
{
    template <typename T> class Point2
    {
        public:
            typedef T value_type;

            Point2();
            Point2(T _x, T _y);
            Point2(const Point2& pt);

            auto calcNormSqr() const;
            auto calcNorm() const;

            Point2& operator=(const Point2& p);

            T x, y;
    };

    using Point2D = Point2<double>;
    using Point2F = Point2<float>;
    using Point2I = Point2<int>;

    using Point2DVector = std::vector<Point2D>;
    using Point2FVector = std::vector<Point2F>;
    using Point2IVector = std::vector<Point2I>;

    template <typename T> class Point3
    {
        public:
            typedef T value_type;

            Point3();
            Point3(T _x, T _y, T _z);
            Point3(const Point3& p);

            auto calcNormSqr() const;
            auto calcNorm() const;

            Point3& operator=(const Point3& p);

            T x, y, z;
    };

    using Point3D = Point3<double>;
    using Point3F = Point3<float>;

    using Point3DVector = std::vector<Point3D>;
    using Point3FVector = std::vector<Point3F>;
}

#include "point.hpp"

#endif 